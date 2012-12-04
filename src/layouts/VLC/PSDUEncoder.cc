// $Id$

#include "PSDUEncoder.h"
#include "bbManchesterEnc.h"
#include "bb4b6bEnc.h"
#include "De2Bi.h"
#include "vlc_convolutional_coding.h"
#include "vlc_reed_solomon.h"
#include "Interleaver.h"
#include "bbRSEnc.h"
#include "bbCCEnc.h"
#include "LayoutVLC.h"
#include "Puncture.h"
#include <gr_io_signature.h>

PSDUEncoder::PSDUEncoder() :
	gr_block("PSDUEncoder", gr_make_io_signature(1, 1, sizeof(int)), gr_make_io_signature(1, 1, sizeof(int))),
	cpd(0), buf(NULL), seq_num(0)
{
	set_tag_propagation_policy(TPP_DONT);
}
PSDUEncoder::sptr PSDUEncoder::Create()
{
	return sptr(new PSDUEncoder());
}
void PSDUEncoder::SetMHR()
{
	memset(buf, 0, sizeof(int)*40);
	buf[6] = 1; buf[10] = 1;
	int dest_mode[] = {1,0}; // equal for source_mode
	memcpy(buf+12, dest_mode, sizeof(dest_mode));
	for (int i = 0; i < 16; i++)
		buf[24+i] = 1;
	De2Bi::dec2bi(seq_num++, 8, buf+16);
	LayoutVLC::GenerateCRC(buf, buf, ph.PL*8-LayoutVLC::CRC_LENGTH);
}
int PSDUEncoder::ProcessPSDU()
{
	ph.PL += 7;
	int pldlen = ph.PL*8, ors = 0, op = 0, p = 0, occ = 0, *iienc = ienc, *iibi = ibi;
	SetMHR();
	if (mod == LayoutVLC::VPPM)
	{
		if (rate < 3)
		{
			int N = 15, K = 2, GF = 4;
			if (rate == 1)
				K = 4;
			else if (rate == 2)
				K = 7;
			ors = bbRSEnc::OutRS(pldlen, GF, N, K, phy_type);
			p = Puncture::OutPunct(pldlen, GF, N, K);
			op = ors-p;
			vlc_reed_solomon vrs(GF, (phy_type == LayoutVLC::PHY_I ? 0x13 : 0x11), 1, 1, N-K);
			bbRSEnc::Encode(buf, irs, ors, pldlen, ors, GF, N, K, phy_type, &vrs);
			unsigned int S_frame = ceil((double)pldlen/GF), S = N*ceil((double)S_frame/K)-(K-(S_frame%K));
			unsigned int D = ceil(((double)S/N)), S_block = N*D;
			std::vector<unsigned int> ivector, pvector;
			for (int i = 0; i < S_block; i++)
				ivector.push_back((i%D)*N + i/D);
			for (int i = 0; i < p; i++)
				pvector.push_back((N-p+1)*D + (i*D)-1);
			Interleaver::Decode((const int *)irs, iilv, ors, ors, ivector, Interleaver::INTERLEAVING);
			unsigned int dummy = 0;
			Puncture::Encode((const int *)iilv, ipu, op, ors, pvector, dummy);
			De2Bi::Encode((const int *)ipu, ibi, op*GF, GF);
			occ = op*GF*6/4;
		}
		else
		{
			occ = pldlen*6/4;
			iibi = buf;
		}
		bb4b6bEnc::Encode((const int *)iibi, ienc, occ);
	}
	else //ook
	{
		if (rate < 4)
		{
			unsigned int N = 15, K = (rate == 0 ? 7 : 11), GF = 4;
			ors = bbRSEnc::OutRS(pldlen, GF, N, K, phy_type);
			p = Puncture::OutPunct(pldlen, GF, N, K);
			op = ors-p;
			vlc_reed_solomon vrs(GF, (phy_type == LayoutVLC::PHY_I ? 0x13 : 0x11), 1, 1, N-K);
			bbRSEnc::Encode(buf, irs, ors, pldlen, ors, GF, N, K, phy_type, &vrs);
			unsigned int S_frame = ceil((double)pldlen/GF), S = N*ceil((double)S_frame/K)-(K-(S_frame%K));
			unsigned int D = ceil(((double)S/N)), S_block = N*D;
			std::vector<unsigned int> ivector, pvector;
			for (int i = 0; i < S_block; i++)
				ivector.push_back((i%D)*N + i/D);
			for (int i = 0; i < p; i++)
				pvector.push_back((N-p+1)*D + (i*D)-1);
			Interleaver::Decode((const int *)irs, iilv, ors, ors, ivector, Interleaver::INTERLEAVING);
			unsigned int dummy = 0;
			Puncture::Encode((const int *)iilv, ipu, op, ors, pvector, dummy);
			De2Bi::Encode((const int *)ipu, ibi, op*GF, GF);
			if (rate < 3)
			{
				int poly[] = { 0133, 0171, 0165 };
				vlc_convolutional_coding CC(3, 7, poly, op*GF, rate);
				occ = bbCCEnc::OutCC(op*GF, rate);
				CC.encode_punct(ibi, ienc, CC.xor_table, CC.poly, 0, 3, 7, op*GF, 2, CC.punct_matrix);
				if (rate == 0)
				{
					for (int i = occ/2-1; i >= 0; i--)
					{
						ienc[2*i] = ienc[i];
						ienc[2*i+1] = ienc[i];
					}
				}
			}
			else
			{
				occ = op*GF;
				iienc = ibi;
			}
		}
		else
		{
			occ = pldlen;
			iienc = buf;
		}
	}
	bbManchesterEnc::Encode((const int *)iienc, ipld, occ*2, mod);
	std::vector<int> data(ipld, ipld+occ*2);
	payloads.push_back(data);
	phys.push_back(ph);
	return 0;
}
int PSDUEncoder::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &_i, gr_vector_void_star &_o) 
{
	const int *iptr = (const int *)_i[0];
	int *optr = (int *)_o[0];
	int ocpd = no, rtd = 0;
	const uint64_t nwrit = nitems_written(0);
	while (ocpd)
	{
		if (!payloads.size())
			break;
		std::vector<int> pld = payloads[0];
		if (phys[0].PL)
		{
			add_item_tag(0, nwrit+no-ocpd, pmt::pmt_string_to_symbol("PSDU"), pmt::pmt_make_any(phys[0]), pmt::pmt_string_to_symbol(name()));
			phys[0].PL = 0;
		}
		if (pld.size() <= ocpd) //pop and thats all
		{
			memcpy(optr, &pld[0], sizeof(int)*pld.size());
			optr += pld.size();
			rtd += pld.size();
			ocpd -= pld.size();
			payloads.erase(payloads.begin());
			phys.erase(phys.begin());
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
		std::vector<gr_tag_t> tags;
		get_tags_in_range(tags, 0, nread, nread+no, pmt::pmt_string_to_symbol("PSDU"));
		if (cpd) //previous
		{
			int c = std::min(cpd, no);
			memcpy(b, iptr, sizeof(int)*c);
			b += c;	cpd -= c;
			if (!cpd)
			{
				ProcessPSDU();
			}
		}
		for (int t = 0; t < tags.size(); t++)
		{
			ph = boost::any_cast<PHYHdr>(pmt::pmt_any_ref(tags[t].value));
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
			cpd = ph.PL*8;
			SetBuffer(cpd, phy_type, mod, rate);
			int off = tags[t].offset-nread;
			int c = std::min(cpd, no-off);
			memcpy(b, iptr+off, sizeof(int)*c);
			b += c;	cpd -= c;
			if (!cpd) //everything is copied, go!
			{
				ProcessPSDU();
			}
		}
		consume_each(no);
	}
	return rtd;
}
void PSDUEncoder::SetBuffer(int len, LayoutVLC::PHYType phy_type, LayoutVLC::Modulation mod, int rate)
{
	len += 56; //MHR
	if (buf)
		delete [] buf;
	if (phy_type == LayoutVLC::PHY_I)
	{
		if (mod == LayoutVLC::OOK)
		{
			int ors = 0, occ = 0, op = 0, omch = 0;
			if (rate == 0)
			{
				ors = bbRSEnc::OutRS(len, 4, 15, 7, phy_type);
				op = ors-Puncture::OutPunct(len, 4, 15, 7);
				occ = bbCCEnc::OutCC(op*4, rate);
				omch = occ*2;
			}
			else if (rate == 1 || rate == 2)
			{
				ors = bbRSEnc::OutRS(len, 4, 15, 11, phy_type);
				op = ors-Puncture::OutPunct(len, 4, 15, 11);
				occ = bbCCEnc::OutCC(op*4, rate);
				omch = occ*2;
			}
			else if (rate == 3)
			{
				ors = bbRSEnc::OutRS(len, 4, 15, 11, phy_type);
				op = ors-Puncture::OutPunct(len, 4, 15, 11);
				omch = op*4*2;
			}
			else
				omch = len*2;
			buf = new int[len+ors+ors+op+op*4+occ+omch];
			b = buf+40;
			irs = buf+len;
			iilv = irs+ors;
			ipu = iilv+ors;
			ibi = ipu+op;
			ienc = ibi+op*4;
			ipld = ienc+occ;
		}
		else if (mod == LayoutVLC::VPPM)
		{
			int ors = 0, occ = 0, op = 0; //CC is RLL 4b6b
			if (rate == 0)
			{
				ors = bbRSEnc::OutRS(len, 4, 15, 2, phy_type);
				op = ors-Puncture::OutPunct(len, 4, 15, 2);
				occ = op*6;
			}
			else if (rate == 1)
			{
				ors = bbRSEnc::OutRS(len, 4, 15, 4, phy_type);
				op = ors-Puncture::OutPunct(len, 4, 15, 4);
				occ = op*6;
			}
			else if (rate == 2)
			{
				ors = bbRSEnc::OutRS(len, 4, 15, 7, phy_type);
				op = ors-Puncture::OutPunct(len, 4, 15, 7);
				occ = op*6;
			}
			else
				occ = len*6/4;
			buf = new int[len+ors+ors+op+op*4+occ+occ*2];
			b = buf+40;
			irs = buf+len;
			iilv = irs+ors;
			ipu = iilv+ors;
			ibi = ipu+op;
			ienc = ibi+op*4;
			ipld = ienc+occ;
		}
	}
}
