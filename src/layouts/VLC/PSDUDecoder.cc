/* 
 * This file is part of the FlexiCom distribution (https://github.com/polhenarejos/flexicom).
 * Copyright (c) 2012-2020 Pol Henarejos, at Centre Tecnologic de Telecomunicacions de Catalunya (CTTC).
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/* $Id$ */
/* $Format:%ci$ ($Format:%h$) by $Format:%an%$ */

#include "PSDUDecoder.h"
#include "bbManchesterDec.h"
#include "bbManchesterDec_2.h"
#include "bb4b6bDec.h"
#include "Bi2De.h"
#include "vlc_convolutional_coding.h"
#include "vlc_reed_solomon.h"
#include "Interleaver.h"
#include "bbRSDec.h"
#include "bbCCDec.h"
#include "LayoutVLC.h"
#include "Parser.h"
#include <gnuradio/io_signature.h>

PSDUDecoder::PSDUDecoder(LayoutVLC *_ly) :
	gr::block("PSDUDecoder", gr::io_signature::make(1, 1, sizeof(float)), gr::io_signature::make(1, 1, sizeof(unsigned char))),
	cpd(0), buf(NULL), buf_bis(NULL), CRCok(0), CRCnok(0), ly(_ly)
{
	set_tag_propagation_policy(TPP_DONT);
}
PSDUDecoder::sptr PSDUDecoder::Create(LayoutVLC *_ly)
{
	return sptr(new PSDUDecoder(_ly));
}
int PSDUDecoder::ProcessPSDU()
{
	int pldlen = ph.PL*8, len = LayoutVLC::GetModulatedResources(phy_type, mod, rate, pldlen), *iibi = ibi, *iipld = ipld;
	if (mod == LayoutVLC::VPPM)
	{
		bbManchesterDec::Decode((const float *)buf, idec, len, 1, 0);
		bb4b6bDec::Decode((const int *)idec, ibi, len*2/3);
		if (rate < 3)
		{
			int obi = len*1/6, N = 15, K = 2;
			if (rate == 1)
				K = 4;
			else if (rate == 2)
				K = 7;
			unsigned int S_frame = ceil((double)ph.PL*2), S = N*ceil((double)S_frame/K)-(K-(S_frame%K));
			unsigned int D = ceil(((double)S/N)), S_block = N*D;
			std::vector<unsigned int> ivector;
			for (int i = 0; i < S_block; i++)
				ivector.push_back((i%D)*N + i/D);
			Bi2De::Decode((const int *)ibi, iilv, obi, 4);
			Interleaver::Decode((const int *)iilv, irs, obi, obi, ivector, Interleaver::DEINTERLEAVING);
			vlc_reed_solomon vrs(4, (phy_type == LayoutVLC::PHY_I ? 0x13 : 0x11), 1, 1, N-K);
			bbRSDec::Decode((const int *)irs, ipld, pldlen, obi, pldlen, N, K, 4, phy_type, &vrs);
		}
		else
			iipld = ibi;
	}
	else //ook
	{
		int omhlen, icclen, occlen;
		if (rate !=0)
		{
			bbManchesterDec::Decode((const float *)buf, idec, len/2, 0, (rate > 2 ? 0 : 1));
			omhlen = len/2, icclen = omhlen, occlen = omhlen;
		}
		else //rate == 0
		{
			bbManchesterDec_2::Decode((const float *)buf, idec, len/4);
			omhlen = len/4, icclen = omhlen, occlen = omhlen;
		}
		if (rate >= 0 && rate <= 2)
		{
			int poly[] = { 0133, 0171, 0165 };
			vlc_convolutional_coding CC(3, 7, poly, icclen, rate);
			occlen = bbCCDec::OutCC(icclen, 7, rate);
			CC.decode_punct(idec, ibi, occlen, 7, 3, CC.no_states, CC.output_reverse_int, icclen, CC.ones, 2, CC.punct_matrix);
		}
		else
			iibi = idec;
		if (rate < 4)
		{
			unsigned int N = 15, K = (rate == 0 ? 7 : 11);
			unsigned int S_frame = ceil((double)ph.PL*2), S = N*ceil((double)S_frame/K)-(K-(S_frame%K));
			unsigned int D = ceil(((double)S/N)), S_block = N*D;
			std::vector<unsigned int> ivector;
			for (int i = 0; i < S_block; i++)
				ivector.push_back((i%D)*N + i/D);
			Bi2De::Decode((const int *)iibi, iilv, occlen/4, 4);
			Interleaver::Decode((const int *)iilv, irs, occlen/4, occlen/4, ivector, Interleaver::DEINTERLEAVING);
			vlc_reed_solomon vrs(4, (phy_type == LayoutVLC::PHY_I ? 0x13 : 0x11), 1, 1, N-K);
			bbRSDec::Decode((const int *)irs, ipld, pldlen, occlen/4, pldlen, N, K, 4, phy_type, &vrs);
		}
		else
			iipld = idec;
	}
	if (LayoutVLC::CheckCRC(iipld, pldlen)) //crc ok!!
	{
		std::vector<unsigned char> data;
		for (int n = 0; n < pldlen-LayoutVLC::CRC_LENGTH; n += 8)
			data.push_back((unsigned char)Bi2De::bi2dec(iipld+n, 8));
		//printf("PSDU OK!\n");
		//Parser::PSDUParser(&data[0]);
		ly->EmitChangeMetric((QLabel *)ly->gridErrors->itemAtPosition(4,1)->widget(), QString::number(++CRCok));
		payloads.push_back(data);
		phys.push_back(ph);
		return data.size();
	}
	else
		ly->EmitChangeMetric((QLabel *)ly->gridErrors->itemAtPosition(4,3)->widget(), QString::number(++CRCnok));
	return 0;
}
int PSDUDecoder::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &_i, gr_vector_void_star &_o) 
{
	const float *iptr = (const float *)_i[0];
	unsigned char *optr = (unsigned char *)_o[0];
	const uint64_t nread = nitems_read(0);
	std::vector<gr::tag_t> tags;
	get_tags_in_range(tags, 0, nread, nread+no, pmt::string_to_symbol("PSDU"));
	if (cpd) //previous
	{
		int c = std::min(cpd, no);
		memcpy(b, iptr, sizeof(float)*c);
		b += c;	cpd -= c;
		if (!cpd)
		{
			ProcessPSDU();
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
		memcpy(b, iptr+off, sizeof(float)*c);
		b += c;	cpd -= c;
		if (!cpd) //everything is copied, go!
		{
			ProcessPSDU();
		}
	}
	int ocpd = no, rtd = 0;
	const uint64_t nwrit = nitems_written(0);
	while (ocpd)
	{
		if (!payloads.size())
			break;
		std::vector<unsigned char> pld = payloads[0];
		if (phys[0].PL)
		{
			add_item_tag(0, nwrit+no-ocpd, pmt::string_to_symbol("PSDU"), pmt::make_any(phys[0]), pmt::string_to_symbol(name()));
			phys[0].PL = 0;
		}
		if (pld.size() <= ocpd) //pop and thats all
		{
			memcpy(optr, &pld[0], sizeof(unsigned char)*pld.size());
			optr += pld.size();
			rtd += pld.size();
			ocpd -= pld.size();
			payloads.erase(payloads.begin());
			phys.erase(phys.begin());
		}
		else
		{
			memcpy(optr, &pld[0], sizeof(unsigned char)*ocpd);
			std::vector<unsigned char> d(pld.begin()+ocpd, pld.end());
			payloads.at(0) = d;
			optr += ocpd;
			rtd += ocpd;
			ocpd -= ocpd;
		}
	}
	consume_each(no);
	return rtd;
}
void PSDUDecoder::SetBuffer(int len, LayoutVLC::PHYType phy_type, LayoutVLC::Modulation mod, int rate)
{
	if (buf)
		delete [] buf;
	if (buf_bis)
		delete [] buf_bis;
	if (phy_type == LayoutVLC::PHY_I)
	{
		if (mod == LayoutVLC::OOK)
		{
			int ors = 0, occ = 0;
			if (rate == 0)
			{
				occ = bbCCDec::OutCC(len/4, 7, rate); 
				ors = bbRSDec::OutRS(occ/4, 15, 7, 4);
			}
			else if (rate == 1 || rate == 2)
			{
				occ = bbCCDec::OutCC(len/2, 7, rate);
				ors = bbRSDec::OutRS(occ/4, 15, 11, 4);
			}
			else if (rate == 3)
			{
				ors = bbRSDec::OutRS(len/8, 15, 11, 4);
				occ = len/2;
			}
			b = buf = new float[len];
			buf_bis = new int[len/2+occ+occ/4+occ/4+ors];
			//idec = buf+len;
			idec = buf_bis;
			ibi = idec+len/2;
			iilv = ibi+occ;
			irs = iilv+occ/4;
			ipld = irs+occ/4;
		}
		else if (mod == LayoutVLC::VPPM)
		{
			int ors = 0, occ = (len/2)*4/6; //CC is RLL 4b6b
			if (rate == 0)
				ors = bbRSDec::OutRS(occ/4, 15, 2, 4);
			else if (rate == 1)
				ors = bbRSDec::OutRS(occ/4, 15, 4, 4);
			else if (rate == 2)
				ors = bbRSDec::OutRS(occ/4, 15, 7, 4);
			b = buf = new float[len];
			buf_bis = new int[len/2+occ+occ/4+occ/4+ors];
			//idec = buf+len;
			idec = buf_bis;
			ibi = idec+len/2;
			iilv = ibi+occ;
			irs = iilv+occ/4;
			ipld = irs+occ/4;
		}
	}
}
