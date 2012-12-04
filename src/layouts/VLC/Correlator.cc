// $Id$

#include "Correlator.h"
#include "LayoutVLC.h"
#include <volk/volk.h>
#include <gnuradio/malloc16.h>
#include <gr_io_signature.h>

Correlator::Correlator(unsigned int _ov, LayoutVLC *_ly, float _th) :
	gr_block("Correlator", gr_make_io_signature(1, 1, sizeof(float)), gr_make_io_signature(1, 1, sizeof(float))),
	pattern(-1), cpd(0), th(_th), ov(_ov), strike(false), ly(_ly)
{
	float _TDP[4][60] = { { 1,1,1,1,-1,1,-1,1,1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,1,-1,1,-1,-1,1,1,-1,1,1,1,1,1,1,1,-1,1,-1,1,1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,1,-1,1,-1,-1,1,1,-1,1,1,1 },
					  { -1,-1,1,-1,1,1,1,-1,1,1,1,1,1,1,-1,1,1,-1,1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,1,-1,-1,1,-1,1,1,1,-1,1,1,1,1,1,1,-1,1,1,-1,1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,1 },
					  { 1,-1,-1,1,1,-1,-1,-1,-1,-1,1,-1,-1,1,1,-1,1,1,-1,-1,1,1,1,1,1,-1,1,1,-1,-1,1,-1,-1,1,1,-1,-1,-1,-1,-1,1,-1,-1,1,1,-1,1,1,-1,-1,1,1,1,1,1,-1,1,1,-1,-1 },
   					  { -1,1,-1,-1,-1,-1,1,1,-1,1,-1,-1,1,-1,1,1,-1,1,1,1,1,-1,-1,1,-1,1,1,-1,1,-1,-1,1,-1,-1,-1,-1,1,1,-1,1,-1,-1,1,-1,1,1,-1,1,1,1,1,-1,-1,1,-1,1,1,-1,1,-1 } };
	//ov = 4; //oversampler factor
	siz = 60*ov;
	copy = 60+1008;
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
	//set_output_multiple(siz);
	//set_tag_propagation_policy(TPP_DONT);
}
Correlator::sptr Correlator::Create(unsigned int _ov, LayoutVLC *_ly, float _th)
{
	return sptr(new Correlator(_ov, _ly, _th));
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
	if (no > siz*2)
		no = siz*2;
	if (!cpd)
	{
		unsigned int idx = 0, corr = no;
		float *C = (float *)malloc16Align(sizeof(float)*corr*(pattern == -1 ? 8 : 1));
		if (pattern == -1)
		{
			for (int t = 0; t < 2; t++)
			{
				for (int i = 0; i < 4; i++)
				{
					float *tC = (float *)malloc16Align(sizeof(float)*corr);
					Correlate(iptr, TDP[t*4+i], tC, corr, t+1);
					memcpy(C+(t*4+i)*corr, tC, sizeof(float)*corr);
					free16Align(tC);
				}
			}
		}
		else
			Correlate(iptr, TDP[pattern], C, corr, vppm);
		volk_32f_index_max_16u_a(&idx, C, (pattern == -1 ? corr*8 : corr));
		if ((C[idx] > 1 && C[idx] > th*2) || (C[idx] <= 1 && C[idx] > th))
		{
			if (!strike)
			{
				strike = true;
				consume_each(idx%no);
				memcpy(optr, iptr, sizeof(float)*(idx%no));
				free16Align(C);
				return idx%no;
			}
			else
			{
				if (pattern == -1)
				{
					vppm = idx/(4*corr)+1;
					pattern = idx/corr;
				}
				//printf("Found sample %f at %d (%d) [%s]\n", C[idx], idx%no, pattern, vppm == 1 ? "OOK" : "VPPM");
				cpd = copy;
				o = idx%corr;
				add_item_tag(0, nitems_written(0)+o+siz*vppm, pmt::pmt_string_to_symbol("SyncPeak"), pmt::PMT_T, pmt::pmt_string_to_symbol(name()));
				strike = false;
				ly->mtx.lock();
				ly->syncs++;
				ly->mtx.unlock();
			}
		}
		else
			strike = false;
		free16Align(C);
	}
	//memcpy(optr, iptr, sizeof(float)*no);
	if (cpd)
	{
		rtd = std::min(cpd, (unsigned int)no);
		cpd -= rtd;
	}
	else
		rtd = no;
	memcpy(optr, iptr, sizeof(float)*rtd);
	//Grab SNR tags
	const uint64_t nread = nitems_read(0);
	std::vector<gr_tag_t> tags;
	/*get_tags_in_range(tags, 0, nread, nread+ni[0], pmt::pmt_string_to_symbol("snr"));
	if (tags.size())
		ly->EmitChangeMetric((QLabel *)ly->gridMeas->itemAtPosition(0, 1)->widget(), QString::number(pmt::pmt_to_double(tags[0].value), 'g', 3));
	get_tags_in_range(tags, 0, nread, nread+ni[0], pmt::pmt_string_to_symbol("power"));
	if (tags.size())
		ly->EmitChangeMetric((QLabel *)ly->gridMeas->itemAtPosition(1, 1)->widget(), QString::number(pmt::pmt_to_double(tags[0].value), 'g'));
		*/
	static bool freset = false;
	get_tags_in_range(tags, 0, nread, nread+no, pmt::pmt_string_to_symbol("rx_time"));
	if (tags.size())
	{
		if (freset)
		{
			ly->SendReport();
			exit(-1);
		}
		else
			freset = true;
	}
	consume_each(rtd);
	return rtd;
}
