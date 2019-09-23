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

#include "compat.h"
#include "Rx80211b.h"
#include "BBN_PLCP.h"
#include "BBN_DPSKDemod.h"
#include "BBN_Slicer.h"
#include "QtBlock.h"
#include "Layout80211b.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/filter/pfb_arb_resampler_ccf.h>
#include <gnuradio/filter/fir_filter_blk.h>
#include <vector>
#include <QtGlobal>
#include <iostream>
#include <gnuradio/blocks/complex_to_real.h>

#include "bbMatlab.h"

Rx80211bThread::Rx80211bThread(gr::msg_queue::sptr _queue) :
	queue(_queue)
{
}
PACK(
struct 
{
	unsigned short fc;
	unsigned short id;
	unsigned char add1[6];
	unsigned char add2[6];
	unsigned char add3[6];
	unsigned short sc;
}, mac_header);
PACK(
struct 
{
	unsigned protocol:2;
	unsigned type:2;
	unsigned subtype:4;
	unsigned to_ds:1;
	unsigned from_ds:1;
	unsigned more_frag:1;
	unsigned retry:1;
	unsigned pwr_mgt:1;
	unsigned more_data:1;
	unsigned wep:1;
	unsigned order:1;
} , frame_control);
PACK(
struct 
{
	unsigned char timestamp[8];
	unsigned char beacon_interval[2];
	unsigned char cap_info[2];
} , beacon_header);
void Rx80211bThread::run()
{
	while (1)
	{
		gr::message::sptr mesg = queue->delete_head();
		if (mesg)
		{
			uint size = sizeof(oob_hdr_t);
			const char *packet = (const char *)mesg->msg(), *packet_data = packet+size;
			oob_hdr_t *oob = (oob_hdr_t *)packet;
			char addr1[18], addr2[18], addr3[18];
			mac_header *p = (mac_header *)(packet_data);
			sprintf(addr1, "%02x:%02x:%02x:%02x:%02x:%02x", p->add1[0], p->add1[1], p->add1[2], p->add1[3], p->add1[4], p->add1[5]);
			char ssid[64];
			ssid[0] = 0;	
    		frame_control *control = (frame_control *) &p->fc;
    		std::cout << "PKT Header" << std::endl;
			std::cout	<< "\tlen=" << (unsigned short)oob->length << std::endl
						<< "\tRSSI=" << (signed short)oob->rssi << std::endl
						<< "\tRate=" << oob->rate/10 << "Mbps" << std::endl;
			std::cout << "\tProtocol version=0x" << std::hex << control->protocol << ", Type=0x" << control->type << ", Subtype=0x" << control->subtype << std::dec << std::endl;
			std::cout<< "\tDestAddr=" << addr1 << std::endl;
			if ((control->protocol == 0) && (control->type == 0) && (control->subtype == 4))  // beacon frame
    		{        
				sprintf(addr2, "%02x:%02x:%02x:%02x:%02x:%02x", p->add2[0], p->add2[1], p->add2[2], p->add2[3], p->add2[4], p->add2[5]);
				sprintf(addr3, "%02x:%02x:%02x:%02x:%02x:%02x", p->add3[0], p->add3[1], p->add3[2], p->add3[3], p->add3[4], p->add3[5]);
				std::cout<< "\tSourceAddr=" << addr2 << std::endl
						<< "\tBSSID=" << addr3 << std::endl;
				//		<< "\tSeqControl=" << p->sc << std::endl
				//		<< "\tCRC Payload=0x" << std::hex << *((unsigned long *)&packet_data[size_d-4])<< std::dec << std::endl;
				char *temp = (char *) (packet + sizeof(mac_header) +sizeof(beacon_header));
				memset(ssid, 0, sizeof(ssid));
				memcpy (ssid, &temp[2], temp[1]);
				if (ssid[0])
					std::cout << "\tSSID=" << ssid << std::endl;
			}
		}
		//else
		//	sleep(1);
	}
}

Rx80211b::Rx80211b(Layout80211b *_ly) :
	gr::hier_block2("Rx80211b", gr::io_signature::make(1, 1, sizeof(gr_complex)), gr::io_signature::make(0, 0, 0)), 
	ly(_ly)
{
	msgq = gr::msg_queue::make();
	rxth = new Rx80211bThread(msgq);
	rxth->start();
	uint chip_rate = 10e6, interpolate_rate = 11, decimation_rate = 10;
	double sample_rate = chip_rate * 11;
	uint spb = 8;
	std::vector<float> rate_taps = gr::filter::firdes::low_pass_2(15, sample_rate, sample_rate / (2 * 11), .25 * sample_rate / 11, 9.f);
	gr::filter::pfb_arb_resampler_ccf::sptr resampler = gr::filter::pfb_arb_resampler_ccf::make((double)interpolate_rate/decimation_rate, rate_taps);
	std::vector<float> filter_taps = BarkerTaps(spb);
	gr::filter::fir_filter_ccf::sptr filter = gr::filter::fir_filter_ccf::make(1, filter_taps);
	BBN_Slicer::sptr slicer = BBN_Slicer::Create(11, 16);
	BBN_DPSKDemod::sptr dpsk = BBN_DPSKDemod::Create();
	BBN_PLCP::sptr plcp = BBN_PLCP::Create(msgq);
	Qt1D::sptr qt1d = Qt1D::Create(ly->pl_osc);
	gr::blocks::complex_to_real::sptr re = gr::blocks::complex_to_real::make();
	connect(self(), 0, resampler, 0);
	connect(resampler, 0, filter, 0);
	connect(filter, 0, slicer, 0);
	connect(slicer, 0, dpsk, 0);
	if (0)
	{
		bbMatlab::sptr gm = bbMatlab::Create("m.txt", sizeof(gr_complex));
		connect(slicer, 0, gm, 0);
	}
	connect(slicer, 0, re, 0);
	connect(re, 0, qt1d, 0);
	connect(dpsk, 0, plcp, 0);
	connect(dpsk, 1, plcp, 1);
}
Rx80211b::sptr Rx80211b::Create(Layout80211b *_ly)
{
	return gnuradio::get_initial_sptr(new Rx80211b(_ly));
}
Rx80211b::~Rx80211b()
{
	stop();
}
void Rx80211b::stop()
{
	if (rxth)
	{
		rxth->terminate();
		rxth->wait();
		delete rxth;
		rxth = NULL;
	}
}
std::vector<float> Rx80211b::BarkerTaps(int sample_rate)
{
	int total_taps = sample_rate * 11 * 3;
	int mid_point = total_taps / 2;
	std::vector<float> rrc(total_taps);
	float Barker[] = {1, -1, 1, 1, -1, 1, 1, 1, -1, -1, -1};
	std::vector<float> result(sample_rate);
	int i, j;
	float maxVal;
	float *ff = new float[total_taps];
	sinc(total_taps, sample_rate, ff);
	for(i=0; i<sample_rate; ++i)
		result[i] = 0;
	/* Convolve expanded Barker with the sinc pulse */
	for(i=0; i<11; ++i) 
	{
		for(j=0; j<sample_rate; ++j) 
		{
			result[j] += ff[i*sample_rate + mid_point - j*11] * 
			Barker[i];
		}
	}
	/* Normalize result so maximum amplitude is 1. */
	maxVal = 0;
	for(i=0; i<sample_rate; ++i) 
	{
		if(fabsf(result[i]) > maxVal)
			maxVal = fabs(result[i]);
	}
	if(maxVal != 0) 
	{
		for(i=0; i<sample_rate; ++i)
			result[i] /= maxVal;
	}
	delete [] ff;
	return result;
}
#define M_PI 3.14159265358979323846
int Rx80211b::sinc(int numSamples, float period, float *result) 
{
	int i;
	float f;
	if(numSamples < 2)
		return -1;
	f = (float)(-numSamples / 2);
	for(i=0; i<numSamples; ++i) 
	{
		if(f == 0)
			result[i] = 1;
		else
			result[i] = period * sin(M_PI * f / period) / (f * M_PI);
		f += 1.0;
	}
	return 0;
}
