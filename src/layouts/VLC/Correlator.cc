// $Id$

#include "Correlator.h"
#include "LayoutVLC.h"
#include <volk/volk.h>
#include <gnuradio/malloc16.h>
#include <gr_io_signature.h>

Correlator::Correlator(int _copy, unsigned int _ov, LayoutVLC *_ly, float _th) :
	gr_block("Correlator", gr_make_io_signature(1, 1, sizeof(float)), gr_make_io_signature(1, 1, sizeof(float))),
	pattern(-1), copy(_copy), cpd(0), th(_th), ov(_ov), strike(false), ly(_ly)
{
	float _TDP[4][60] = { { 1,1,1,1,-1,1,-1,1,1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,1,-1,1,-1,-1,1,1,-1,1,1,1,1,1,1,1,-1,1,-1,1,1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,1,-1,1,-1,-1,1,1,-1,1,1,1 },
					  { -1,-1,1,-1,1,1,1,-1,1,1,1,1,1,1,-1,1,1,-1,1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,1,-1,-1,1,-1,1,1,1,-1,1,1,1,1,1,1,-1,1,1,-1,1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,1 },
					  { 1,-1,-1,1,1,-1,-1,-1,-1,-1,1,-1,-1,1,1,-1,1,1,-1,-1,1,1,1,1,1,-1,1,1,-1,-1,1,-1,-1,1,1,-1,-1,-1,-1,-1,1,-1,-1,1,1,-1,1,1,-1,-1,1,1,1,1,1,-1,1,1,-1,-1 },
   					  { -1,1,-1,-1,-1,-1,1,1,-1,1,-1,-1,1,-1,1,1,-1,1,1,1,1,-1,-1,1,-1,1,1,-1,1,-1,-1,1,-1,-1,-1,-1,1,1,-1,1,-1,-1,1,-1,1,1,-1,1,1,1,1,-1,-1,1,-1,1,1,-1,1,-1 } };
	//ov = 4; //oversampler factor
	siz = 60*ov;
	copy *= ov;
	for (int t = 0; t < 2; t++)
	{
		for (int i = 0; i < 4; i++)
		{
			int s = ov*(t+1);
			TDP[i+t*4] = (float *)malloc16Align(sizeof(float)*60*s);
			for (int j = 0; j < 60; j++)
				std::fill_n(TDP[i+t*4]+j*s, s, _TDP[i][j]);
		}
	}
	//set_alignment(volk_get_alignment()/sizeof(float));
	set_output_multiple(siz);
}
Correlator::sptr Correlator::Create(int _copy, unsigned int _ov, LayoutVLC *_ly, float _th)
{
	return sptr(new Correlator(_copy, _ov, _ly, _th));
}
Correlator::~Correlator()
{
	for (int i = 0; i < 8; i++)
		free16Align(TDP[i]);
}
void Correlator::forecast(int no, gr_vector_int &ni)
{
	ni[0] = no+siz*2;
}
void Correlator::Correlate(const float *iptr, float *tD, float *tC, int no, int v)
{
	float *tN = (float *)malloc16Align(sizeof(float)*no);
	//if (is_unaligned()) 
	{
		volk_32f_x2_dot_prod_32f_u(tC, iptr, tD, siz*v);
		volk_32f_x2_dot_prod_32f_u(tN, iptr, iptr, siz*v);
	}
	/*else
	{
		volk_32f_x2_dot_prod_32f_a(tC, iptr, tD, siz*v);
		volk_32f_x2_dot_prod_32f_a(tN, iptr, iptr, siz*v);
	}*/
	for (int n = 1; n < no; n++)
	{
		volk_32f_x2_dot_prod_32f_u(tC+n, iptr+n, tD, siz*v);
		volk_32f_x2_dot_prod_32f_u(tN+n, iptr+n, iptr+n, siz*v);
	}
	volk_32f_x2_multiply_32f_a(tC, tC, tC, no);
	volk_32f_s32f_multiply_32f_a(tN, tN, siz*v/2, no);
	volk_32f_x2_divide_32f_a(tC, tC, tN, no);
	free16Align(tN);
}
int Correlator::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &_i, gr_vector_void_star &_o) 
{
	const float *iptr = (const float *)_i[0]; 
	float *optr = (float *)_o[0];
	unsigned int o = 0, rtd = 0;
	if (!cpd)
	{
		if (strike)
			no = siz;
		unsigned int idx = 0;
		float *C = (float *)malloc16Align(sizeof(float)*no*(pattern == -1 ? 8 : 1));
		if (pattern == -1)
		{
			for (int t = 0; t < 2; t++)
			{
				for (int i = 0; i < 4; i++)
				{
					float *tC = (float *)malloc16Align(sizeof(float)*no);
					Correlate(iptr, TDP[t*4+i], tC, no, t+1);
					memcpy(C+(t*4+i)*no, tC, sizeof(float)*no);
					free16Align(tC);
				}
			}
		}
		else
			Correlate(iptr, TDP[pattern], C, no, vppm);
		volk_32f_index_max_16u_a(&idx, C, (pattern == -1 ? no*8 : no));
		if ((C[idx] > 1 && C[idx] > th*2) || (C[idx] <= 1 && C[idx] > th))
		{
			if (!strike)
			{
				strike = true;
				consume_each(idx%no);
				return 0;
			}
			else
			{
				if (pattern == -1)
				{
					vppm = idx/(4*no)+1;
					pattern = idx/no;
				}
				//printf("Found sample %f at %d (%d) [%s]\n", C[idx], idx%no, pattern, vppm == 1 ? "OOK" : "VPPM");
				cpd = copy;
				o = idx%no;
				strike = false;
				ly->EmitChangeMetric((QLabel *)ly->gridSynch->itemAtPosition(0, 1)->widget(), QString("<b><font color=green>Ok!</font></b>"));
			}
		}
		else
		{
			if (strike)
				strike = false;
		}
		free16Align(C);
	}
	if (cpd)
	{
		rtd = std::min(cpd, no-o);
		memcpy(optr, iptr+o, sizeof(float)*rtd);
		cpd -= rtd;
	}
	else
		ly->EmitChangeMetric((QLabel *)ly->gridSynch->itemAtPosition(0, 1)->widget(), QString("<b><font color=red>Fail</font></b>"));
	//Grab SNR tags
	const uint64_t nread = nitems_read(0);
	std::vector<gr_tag_t> tags;
	get_tags_in_range(tags, 0, nread, nread+ni[0], pmt::pmt_string_to_symbol("snr"));
	if (tags.size())
		ly->EmitChangeMetric((QLabel *)ly->gridMeas->itemAtPosition(0, 1)->widget(), QString::number(pmt::pmt_to_double(tags[0].value)));
	if (o+rtd)
		consume_each(o+rtd);
	else //didnt found anything
		consume_each(no);
	return rtd;
}
