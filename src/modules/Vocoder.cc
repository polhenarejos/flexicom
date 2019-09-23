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

#include "Vocoder.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
extern "C" {
#include <gsm/gsm.h>
}
#include <malloc16.h>

VocoderEncoder::VocoderEncoder() :
	gr::sync_decimator("VocoderEncoder", gr::io_signature::make(1, 1, sizeof(float)), gr::io_signature::make(1, 1, sizeof(gsm_frame)), GSM_SAMPLES_PER_FRAME),
	reset(0)
{
	dgsm = gsm_create();
}
VocoderEncoder::~VocoderEncoder()
{
	gsm_destroy(dgsm);
}
VocoderEncoder::sptr VocoderEncoder::Create()
{
	return sptr(new VocoderEncoder());
}
int VocoderEncoder::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	const float *iptr = (const float *)_i[0];
	float *tN = (float *)malloc16Align(sizeof(float)*no*GSM_SAMPLES_PER_FRAME);
	short *s = (short *)malloc16Align(sizeof(short)*no*GSM_SAMPLES_PER_FRAME), *siptr = s;
	if (is_unaligned())
		volk_32f_s32f_multiply_32f_u(tN, iptr, 32767., no*GSM_SAMPLES_PER_FRAME);
	else
		volk_32f_s32f_multiply_32f_a(tN, iptr, 32767., no*GSM_SAMPLES_PER_FRAME);
	volk_32f_s32f_convert_16i_a(siptr, tN, 1., no*GSM_SAMPLES_PER_FRAME);
	unsigned char *optr = (unsigned char *)_o[0];
	for (int n = 0; n < no; n++)
	{
		if ((reset = reset+1)%100 == 0)
		{
			gsm_destroy(dgsm);
			dgsm = gsm_create();
			add_item_tag(0, nitems_written(0)+n, pmt::string_to_symbol("VocoderReset"), pmt::T, pmt::string_to_symbol(name()));
		}
		gsm_encode(dgsm, const_cast<short *>(siptr), optr);
		siptr += GSM_SAMPLES_PER_FRAME;
		optr += sizeof(gsm_frame);
	}
	free16Align(tN);
	free16Align(s);
	return no;
}

VocoderDecoder::VocoderDecoder() :
	gr::sync_interpolator("VocoderDecoder", gr::io_signature::make(1, 1, sizeof(gsm_frame)), gr::io_signature::make(1, 1, sizeof(float)), GSM_SAMPLES_PER_FRAME)
{
	dgsm = gsm_create();
}
VocoderDecoder::~VocoderDecoder()
{
	gsm_destroy(dgsm);
}
VocoderDecoder::sptr VocoderDecoder::Create()
{
	return sptr(new VocoderDecoder());
}
int VocoderDecoder::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	const unsigned char *iptr = (const unsigned char *)_i[0];
	float *tN = (float *)malloc16Align(sizeof(float)*no);
	short *s = (short *)malloc16Align(sizeof(short)*no), *soptr = s;
	float *optr = (float *)_o[0];
	const uint64_t nread = nitems_read(0);
	std::vector<gr::tag_t> tags;
	get_tags_in_range(tags, 0, nread, nread+no/GSM_SAMPLES_PER_FRAME, pmt::string_to_symbol("VocoderReset"));
	unsigned int o = 0;
	for (int n = 0; n < no; n += GSM_SAMPLES_PER_FRAME)
	{
		if (tags.size() && tags[0].offset == nread+o)
		{
			//printf("Resetting at %d\n",nread+o);
			gsm_destroy(dgsm);
			dgsm = gsm_create();
			tags.erase(tags.begin());
		}
		gsm_decode(dgsm, const_cast<unsigned char *>(iptr), soptr);
		iptr += sizeof(gsm_frame);
		soptr += GSM_SAMPLES_PER_FRAME;
		o++;
	}
	volk_16i_s32f_convert_32f_a(tN, s, 1., no);
	volk_32f_s32f_multiply_32f_a(optr, tN, 1./32767., no);
	free16Align(tN);
	free16Align(s);
	return no;
}
