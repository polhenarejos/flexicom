// $Id$

#include "PHRDecoder.h"
#include "bbManchesterDec.h"
#include "bbManchesterDec_2.h"
#include "bb4b6bDec.h"
#include "Bi2De.h"
#include "vlc_convolutional_coding.h"
#include "vlc_reed_solomon.h"
#include "Interleaver.h"
#include "bbRSDec.h"
#include "LayoutVLC.h"
#include "Parser.h"
#include <gr_io_signature.h>

PHRDecoder::PHRDecoder(LayoutVLC *_ly) :
	gr_sync_block("PHRDecoder", gr_make_io_signature(1, 1, sizeof(float)), gr_make_io_signature(1, 1, sizeof(float))),
	cpd(0), phy_type((LayoutVLC::PHYType)_ly->vlc_var.phy_type), mod((LayoutVLC::Modulation)_ly->vlc_var.mod_type), ly(_ly),
	CRCok(0), CRCnok(0)
{
	//reserver memory as a single memory page
	b = buf = new float[1080]; // with OOK 944
	//b = buf = new int[1080+540+360+90+90+48]; //with OOK is 944+472+112+28+28+48
	buf_bis = new int[540+360+90+90+48]; //with OOK is 944+472+112+28+28+48
	//idec = buf+1080;
	idec = buf_bis;
	ibi = idec+540;
	iilv = ibi+360;
	irs = idec+90;
	ipld = irs+90;
	int poly[] = { 0133 , 0171 , 0165 };
	CC = new vlc_convolutional_coding(3, 7, poly, 472, 0);
	if (phy_type == LayoutVLC::PHY_I)
	{
		RS[0] = new vlc_reed_solomon(4, 0x13, 1, 1, 15-7);
		RS[1] = new vlc_reed_solomon(4, 0x13, 1, 1, 15-2);
	}
	else if (phy_type == LayoutVLC::PHY_II)
		RS[0] = new vlc_reed_solomon(8, 0x11, 1, 1, 64-32);
	for (unsigned int i = 0; i < 30; i++)
		ivector[0].push_back((i%2)*15 + i/2);
	for (unsigned int i = 0; i < 90; i++)
		ivector[1].push_back((i%6)*15 + i/6);
}
PHRDecoder::~PHRDecoder()
{
	delete [] buf;
	delete [] buf_bis;
	delete CC;
	delete RS[0];
	if (phy_type == LayoutVLC::PHY_I)
		delete RS[1];
}
PHRDecoder::sptr PHRDecoder::Create(LayoutVLC *_ly)
{
	return sptr(new PHRDecoder(_ly));
}
bool PHRDecoder::ProcessPHR(PHYHdr *ph)
{
	unsigned char data[4]; //4 bytes
	int phy_type = 0;
	if (mod == LayoutVLC::VPPM)
	{
		bbManchesterDec::Decode((const float *)buf, idec, 540, 1, 0);
		bb4b6bDec::Decode((const int *)idec, ibi, 360);
		Bi2De::Decode((const int *)ibi, iilv, 90, 4);
		Interleaver::Decode((const int *)iilv, irs, 90, 90, ivector[1], Interleaver::DEINTERLEAVING);
		bbRSDec::Decode((const int *)irs, ipld, 48, 90, 48, 15, 2, 4, phy_type, RS[1]);
	}
	else //ook
	{
		bbManchesterDec_2::Decode((const float *)buf, idec, 944/2);
		CC->decode_punct(idec, ibi, 112, 7, 3, CC->no_states, CC->output_reverse_int, 236, CC->ones, 2, CC->punct_matrix);
		Bi2De::Decode((const int *)ibi, iilv, 112, 4);
		Interleaver::Decode((const int *)iilv, irs, 28, 28, ivector[0], Interleaver::DEINTERLEAVING);
		bbRSDec::Decode((const int *)irs, ipld, 48, 28, 48, 15, 7, 4, phy_type, RS[0]);
	}
	if (LayoutVLC::CheckCRC(ipld, 48)) //crc ok!!
	{
		//printf("CRC OK!!\n");
		for (int n = 0; n < 48-LayoutVLC::CRC_LENGTH; n += 8)
			data[n/8] = (unsigned char)LayoutVLC::bi2dec(ipld+n, 8);
		unsigned int d = data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0];
		memcpy(ph, &d, sizeof(PHYHdr));
		ly->EmitChangeMetric((QLabel *)ly->gridErrors->itemAtPosition(3,1)->widget(), QString::number(++CRCok));
		//Parser::PHRParser(d);
		return true;
	}
	else
		ly->EmitChangeMetric((QLabel *)ly->gridErrors->itemAtPosition(3,3)->widget(), QString::number(++CRCnok));
	return false;
}
int PHRDecoder::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o) 
{
	const float *iptr = (const float *)_i[0];
	float *optr = (float *)_o[0];
	memcpy(optr, iptr, sizeof(float)*no);
	const uint64_t nread = nitems_read(0);
	std::vector<gr_tag_t> tags;
	get_tags_in_range(tags, 0, nread, nread+no, pmt::pmt_string_to_symbol("SyncPeak"));
	PHYHdr ph;
	if (cpd) //previous
	{
		int c = std::min(cpd, no);
		memcpy(b, iptr, sizeof(float)*c);
		b += c;	cpd -= c;
		if (!cpd)
		{
			if (ProcessPHR(&ph))
			{
				if (ph.MCS < 5) //tail bits
					offPSDU += 64;
				add_item_tag(0, offPSDU, pmt::pmt_string_to_symbol("PSDU"), pmt::pmt_make_any(ph), pmt::pmt_string_to_symbol(name()));
			}
			b = buf;
		}
	}
	for (int t = 0; t < tags.size(); t++)
	{
		cpd = 944; //PHR length
		if (mod == LayoutVLC::VPPM)
			cpd = 1080;
		offPSDU = tags[t].offset+cpd;
		int off = tags[t].offset-nread;
		int c = std::min(cpd, no-off);
		memcpy(b, iptr+off, sizeof(float)*c);
		b += c;	cpd -= c;
		if (!cpd) //everything is copied, go!
		{
			if (ProcessPHR(&ph))
			{
				if (ph.MCS < 5) //tail bits
					offPSDU += 64;
				add_item_tag(0, offPSDU, pmt::pmt_string_to_symbol("PSDU"), pmt::pmt_make_any(ph), pmt::pmt_string_to_symbol(name()));
			}
			b = buf;
		}
	}
	return no;
}
