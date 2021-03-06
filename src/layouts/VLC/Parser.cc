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

#include "Parser.h"
#include "LayoutVLC.h"
#include <QTextEdit>
#include <gnuradio/io_signature.h>

Parser::Parser(Type _type, LayoutVLC *_ly) :
	gr::block("Parser", gr::io_signature::make(1, 1, sizeof(unsigned char)), gr::io_signature::make(0, 1, sizeof(unsigned char))),
	ic(0), type(_type), PHRData(0x0), ly(_ly), per(0), total(0), prevreset(false), cpd(0)
{
}
Parser::sptr Parser::Create(Type _type, LayoutVLC *_ly)
{
	return sptr(new Parser(_type, _ly));
}
void Parser::PHRParser(unsigned int PHRData)
{
	PhyHdr *ph = (PhyHdr *)&PHRData;
	printf("---------------------\nPHR HEADER (%X)\n| Burst_mode: %d\n", PHRData, ph->B);
	printf("| Channel_number: %d\n", ph->CN);
	printf("| MCSID: %d\n", ph->MCS);
	printf("| PSDU_length: %d bytes\n", ph->PL);
	printf("| Dimmed_OOK_extension: %d\n", ph->DO);
}
void Parser::PSDUParser(unsigned char *MHR)
{
	FrameControl *fc = (FrameControl *)MHR;
	printf("+++++++++++++++++++++\n+ PSDU HEADER(%X-%X-%X-%X-%X)\n+ Frame version: %d\n", MHR[0], MHR[1], MHR[2], MHR[3], MHR[4], fc->FV);
	printf("+ Frame type: %d\n", fc->FT);
	printf("+ Security enabled: %d\n", fc->SE);
	printf("+ Frame pending: %d\n", fc->FP);
	printf("+ Ack request: %d\n", fc->AR);
	printf("+ Dest. addr. mode: %d\n", fc->DAM);
	printf("+ Source addr. mode: %d\n", fc->SAM);
	printf("+ Sequence number: %d\n", MHR[2]);
	printf("+ Dest. Address: %X.%X\n", MHR[3], MHR[4]);
}
#include <QMutex>
extern QMutex mtx;
int Parser::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	const unsigned char *iptr = (const unsigned char *)_i[0];
	unsigned char *optr = NULL;
	if (_o.size())
		optr = (unsigned char *)_o[0];
	int rtd = 0;
	if (type == PHR)
	{
		for (int n = 0; n < no; n++)
		{
			PHRData |= ((unsigned int )*iptr++ << (0x8)*ic);
			if ((ic = (ic+1)%4) == 0)
			{
				//PHRParser(PHRData);
				PHRData = 0x0;
			}
		}
	}
	else if (type == PSDU)
	{
		const uint64_t nread = nitems_read(0);
		std::vector<gr::tag_t> tags;
		get_tags_in_range(tags, 0, nread, nread+no, pmt::string_to_symbol("PSDU"));
		for (int n = 0; n < no; n++)
		{
			if (ic < 5)
			{
				if (ic == 0)
				{
					PHYHdr ph = boost::any_cast<PHYHdr>(pmt::any_ref(tags[0].value));
					psdu_len = ph.PL-2; //minus CRC
					tags.erase(tags.begin());
				}
				MHR[ic] = *iptr;
				//if (ic == 4)
				//	PSDUParser(MHR);
				if (ic == 2)
				{
					if (prevreset)
					{
						unsigned char a = 0x0;
						if (MHR[2] > prevSeq)
							a = (MHR[2]-1-prevSeq);
						else
							a = (0xff-prevSeq+MHR[2]);
						per += a;
						total += a+1;
					}
					else
						prevreset = true;
					//if (MHR[2] != (unsigned char)(prevSeq+1))
					if (per || total)
					{
						double PER = (double)per/total;
						ly->EmitChangeMetric((QLabel *)ly->gridErrors->itemAtPosition(1, 1)->widget(), QString::number(PER));
						ly->EmitChangeMetric((QLabel *)ly->gridErrors->itemAtPosition(2, 1)->widget(), QString::number(per));
						ly->EmitChangeMetric((QLabel *)ly->gridLink->itemAtPosition(2, 1)->widget(), QString::number(total));
					}
					prevSeq = MHR[2];
				}
			}
			else if (ic == 5)
			{
				payload_len = 0x0;
				payload_ver = *iptr & 0x3;
				voip = (*iptr & 0x4);
				ly->mtx.lock();
				ly->bits += 8;
				ly->mtx.unlock();
			}
			else
			{
				if (payload_ver == 0)
				{
					//if (*iptr)
					//	printf("* %d (%d) %d\n",*iptr,ic,psdu_len);
					if (ic == 6)
						payload_len |= *iptr;
					else if (ic == 7)
						payload_len |= (unsigned short)*iptr << 0x8;
					else
					{
						if (voip)
						{
							if (ic < psdu_len-payload_len)
								optr[rtd++] = *iptr;
							else
							{
								payload.append(*iptr);
								if (ic+1 == psdu_len)
								{
									payload.append('\n');
									ly->ChatAppend(payload);
									payload.clear();
								}
							}
						}
						else
						{
							if (payload_len)
							{
								int ric = ic-8;
								if (ric < payload_len)
									payload.append(*iptr);
								else if (ric == payload_len)
								{
									//printf("\n");
									payload.append('\n');
									ly->ChatAppend(payload);
									payload.clear();
								}
							}
						}
					}
				}
				ly->mtx.lock();
				ly->bits += 8;
				ly->mtx.unlock();
			}
			ic = (ic+1)%psdu_len;
			iptr++;
		}
	}
	consume_each(no);
	return rtd;
}
