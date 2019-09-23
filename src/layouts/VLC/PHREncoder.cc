// $Id$

#include "PHREncoder.h"
#include "bbManchesterEnc.h"
#include "bb4b6bEnc.h"
#include "De2Bi.h"
#include "vlc_convolutional_coding.h"
#include "vlc_reed_solomon.h"
#include "Interleaver.h"
#include "bbRSEnc.h"
#include "bbCCEnc.h"
#include "LayoutVLC.h"
#include <gnuradio/io_signature.h>

const int PHREncoder::_TDP[] = { 0,1,0,0,0,0,1,1,0,1,0,0,1,0,1,1,0,1,1,1,1,0,0,1,0,1,1,0,1,0,
								 0,0,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,
								 1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,0,0,1,1,1,1,0,0,1,1,0,0  
	};

PHREncoder::PHREncoder() :
	gr::block("PHREncoder", gr::io_signature::make(1, 1, sizeof(int)), gr::io_signature::make(1, 1, sizeof(int))),
	cpd(0), buf(NULL)
{
	//PHR does not perform any puncture
	phrbuf = new int[54+90+90+360+540]; //with OOK is 48+30+30+120+504+1008
	irs = phrbuf+54;
	iilv = irs+90;
	ibi = iilv+90;
	ienc = ibi+360;
	int poly[] = { 0133 , 0171 , 0165 };
	CC = new vlc_convolutional_coding(3, 7, poly, 120, 0);
	RS[0] = new vlc_reed_solomon(4, 0x13, 1, 1, 15-7);
	RS[1] = new vlc_reed_solomon(4, 0x13, 1, 1, 15-2);
	//RS[2] = new vlc_reed_solomon(8, 0x11, 1, 1, 64-32);
	for (unsigned int i = 0; i < 30; i++)
		ivector[0].push_back((i%2)*15 + i/2);
	for (unsigned int i = 0; i < 90; i++)
		ivector[1].push_back((i%6)*15 + i/6);
	TDP[0] = _TDP;
	TDP[1] = _TDP+30;
	set_tag_propagation_policy(TPP_DONT);
}
PHREncoder::~PHREncoder()
{
	delete [] phrbuf;
	delete CC;
	delete RS[0];
	delete RS[1];
	//delete RS[2];
}
PHREncoder::sptr PHREncoder::Create()
{
	return sptr(new PHREncoder());
}
int PHREncoder::ProcessPHR()
{
	memset(phrbuf, 0, sizeof(int)*54);
	De2Bi::dec2bi(ph.MCS, 6, phrbuf+4);
	De2Bi::dec2bi(ph.PL, 16, phrbuf+10);
	LayoutVLC::GenerateCRC(phrbuf, phrbuf, 32);
	int occ = 0;
	if (mod == LayoutVLC::VPPM)
	{
		bbRSEnc::Encode(phrbuf, irs, 90, 48, 90, 4, 15, 2, phy_type, RS[1]);
		Interleaver::Decode((const int *)irs, iilv, 90, 90, ivector[1], Interleaver::INTERLEAVING);
		De2Bi::Encode((const int *)iilv, ibi, 360, 4);
		bb4b6bEnc::Encode((const int *)ibi, ienc, 540);
		occ = 540;
		memcpy(buf, TDP[1], sizeof(int)*60);
		memcpy(buf+60, TDP[1], sizeof(int)*60);
	}
	else //ook
	{
		bbRSEnc::Encode(phrbuf, irs, 30, 54, 30, 4, 15, 7, phy_type, RS[0]);
		Interleaver::Decode((const int *)irs, iilv, 30, 30, ivector[0], Interleaver::INTERLEAVING);
		De2Bi::Encode((const int *)iilv, ibi, 120, 4);
		CC->encode_punct(ibi, ienc, CC->xor_table, CC->poly, 0, 3, 7, 120, 2, CC->punct_matrix);
		for (int i = 251; i >= 0; i--)
		{
			ienc[2*i] = ienc[i];
			ienc[2*i+1] = ienc[i];
		}
		occ = 504;
		memcpy(buf, TDP[0], sizeof(int)*30);
		memcpy(buf+30, TDP[0], sizeof(int)*30);
	}
	bbManchesterEnc::Encode((const int *)ienc, iphr, occ*2, mod);
	std::vector<int> data(buf, buf+flen);
	payloads.push_back(data);
	return 0;
}
int PHREncoder::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &_i, gr_vector_void_star &_o) 
{
	const int *iptr = (const int *)_i[0];
	int *optr = (int *)_o[0];
	int ocpd = no, rtd = 0;
	while (ocpd)
	{
		if (!payloads.size())
			break;
		std::vector<int> pld = payloads[0];
		if (pld.size() <= ocpd) //pop and thats all
		{
			memcpy(optr, &pld[0], sizeof(int)*pld.size());
			optr += pld.size();
			rtd += pld.size();
			ocpd -= pld.size();
			payloads.erase(payloads.begin());
		}
		else
		{
			memcpy(optr, &pld[0], sizeof(int)*ocpd);
			std::vector<int> d(pld.begin()+ocpd, pld.end());
			payloads.at(0) = d;
			optr += ocpd;
			rtd += ocpd;
			ocpd -= ocpd;
		}
	}
	if (!payloads.size()) //only accept samples if the obuf is empty
	{
		const uint64_t nread = nitems_read(0);
		std::vector<gr::tag_t> tags;
		get_tags_in_range(tags, 0, nread, nread+no, pmt::string_to_symbol("PSDU"));
		if (cpd) //previous
		{
			int c = std::min(cpd, no);
			memcpy(b, iptr, sizeof(int)*c);
			b += c;	cpd -= c;
			if (!cpd)
			{
				ProcessPHR();
			}
		}
		for (int t = 0; t < tags.size(); t++)
		{
			ph = boost::any_cast<PHYHdr>(pmt::any_ref(tags[t].value));
			int MCSID = ph.MCS;
			if (MCSID <= 8)
			{
				phy_type = LayoutVLC::PHY_I;
				if (MCSID <= 4)
				{
					mod = LayoutVLC::OOK;
					rate = MCSID;
				}
				else
				{
					mod = LayoutVLC::VPPM;
					rate = MCSID-5;
				}
			}
			else if (MCSID >= 16 && MCSID <= 29)
			{
				phy_type = LayoutVLC::PHY_II;
				if (MCSID <= 20)
				{
					mod = LayoutVLC::VPPM;
					rate = MCSID-16;
				}
				else
				{
					mod = LayoutVLC::OOK;
					rate = MCSID-21;
				}
			}
			cpd = LayoutVLC::GetModulatedResources(phy_type, mod, rate, ph.PL*8);
			if (mod == LayoutVLC::VPPM)
				cpd *= 2;
			SetBuffer(cpd, phy_type, mod, rate);
			int off = tags[t].offset-nread;
			int c = std::min(cpd, no-off);
			memcpy(b, iptr+off, sizeof(int)*c);
			b += c;	cpd -= c;
			if (!cpd) //everything is copied, go!
			{
				ProcessPHR();
			}
		}
		consume_each(no);
	}
	return rtd;
}
void PHREncoder::SetBuffer(int len, LayoutVLC::PHYType phy_type, LayoutVLC::Modulation mod, int rate)
{
	if (buf)
		delete [] buf;
	int phrlen = 0, tdplen = 0;
	if (phy_type == LayoutVLC::PHY_I)
	{
		if (mod == LayoutVLC::OOK)
		{
			tdplen = 60;
			phrlen = 1008;
		}
		else if (mod == LayoutVLC::VPPM)
		{
			tdplen = 120;
			phrlen = 1080;
		}
		flen = tdplen+phrlen+len;
		buf = new int[flen];
		iphr = buf+tdplen;
		b = iphr+phrlen;
	}
}
