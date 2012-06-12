#include "compat.h"
#include "Rx80211b.h"
#include "BBN_PLCP.h"
#include "BBN_DPSKDemod.h"
#include "BBN_Slicer.h"
#include "QtBlock.h"
#include "Layout80211b.h"
#include <gr_io_signature.h>
#include <gr_firdes.h>
#include <gr_pfb_arb_resampler_ccf.h>
#include <gr_fir_filter_ccf.h>
#include <vector>
#include <QtGlobal>
#include <iostream>
#include <gr_complex_to_xxx.h>

#include "bbMatlab.h"

Rx80211bThread::Rx80211bThread(gr_msg_queue_sptr _queue) :
	queue(_queue)
{
}
PACK(
typedef struct mac_header
{
	unsigned short fc;
	unsigned short id;
	unsigned char add1[6];
	unsigned char add2[6];
	unsigned char add3[6];
	unsigned short sc;
} mac_header;);
PACK(
typedef struct frame_control
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
} frame_control;);
PACK(
typedef struct beacon_header
{
	unsigned char timestamp[8];
	unsigned char beacon_interval[2];
	unsigned char cap_info[2];
} beacon_header;);
void Rx80211bThread::run()
{
	while (1)
	{
		gr_message_sptr mesg = queue->delete_head();
		if (mesg)
		{
			uint size = sizeof(oob_hdr_t), size_d = mesg->length()-size;
			const char *packet = (const char *)mesg->msg(), *packet_data = packet+size;
			oob_hdr_t *oob = (oob_hdr_t *)packet;
			char addr1[18], addr2[18], addr3[18];
			struct mac_header *p = (struct mac_header *)(packet_data);
			sprintf(addr1, "%02x:%02x:%02x:%02x:%02x:%02x", p->add1[0], p->add1[1], p->add1[2], p->add1[3], p->add1[4], p->add1[5]);
			char ssid[64];
			ssid[0] = 0;	
    		struct frame_control *control = (struct frame_control *) &p->fc;
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
				char *temp = (char *) (packet + sizeof(struct mac_header) +sizeof (struct beacon_header));
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
	gr_hier_block2("Rx80211b", gr_make_io_signature(1, 1, sizeof(gr_complex)), gr_make_io_signature(0, 0, 0)), 
	ly(_ly)
{
	msgq = gr_make_msg_queue();
	rxth = new Rx80211bThread(msgq);
	rxth->start();
	uint chip_rate = 10e6, interpolate_rate = 11, decimation_rate = 10;
	double sample_rate = chip_rate * 11;
	uint spb = 8, ntaps = 2*spb-1;
	std::vector<float> rate_taps = gr_firdes::low_pass_2(15, sample_rate, sample_rate / (2 * 11), .25 * sample_rate / 11, 9.f);
	gr_pfb_arb_resampler_ccf_sptr resampler = gr_make_pfb_arb_resampler_ccf((double)interpolate_rate/decimation_rate, rate_taps);
	std::vector<float> filter_taps = BarkerTaps(spb);
	gr_fir_filter_ccf_sptr filter = gr_make_fir_filter_ccf(1, filter_taps);
	BBN_Slicer::sptr slicer = BBN_Slicer::Create(11, 16);
	BBN_DPSKDemod::sptr dpsk = BBN_DPSKDemod::Create();
	BBN_PLCP::sptr plcp = BBN_PLCP::Create(msgq);
	Qt1D::sptr qt1d = Qt1D::Create(ly->pl_osc);
	gr_complex_to_real_sptr re = gr_make_complex_to_real();
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
	return sptr(new Rx80211b(_ly));
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
	int filter_period;
	float maxVal;
	float *ff = new float[total_taps];
	if(sample_rate > 11)
		filter_period = sample_rate;
	else
		filter_period = 11;
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
