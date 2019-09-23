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

#include "Audio.h"
#include "Vocoder.h"
#include <gnuradio/vector_to_stream.h>
#include <gnuradio/stream_to_vector.h>
#include <gnuradio/sig_source_f.h>
#include <gnuradio/audio_source.h>
#include <gnuradio/audio_sink.h>
#include <gnuradio/io_signature.h>

AudioSource::AudioSource(unsigned int _freq) :
	gr::hier_block2("AudioSource", gr::io_signature::make(0, 0, 0), gr::io_signature::make(1, 1, sizeof(unsigned char)))
{
	VocoderEncoder::sptr vocoder = VocoderEncoder::Create();
	gr_vector_to_stream_sptr v2s = gr_make_vector_to_stream(sizeof(unsigned char), 33);
	//connect(gr_make_sig_source_f(6100, GR_SIN_WAVE, 350, 0.1), 0, vocoder, 0);
	connect(audio_make_source(6100), 0, vocoder, 0);
	connect(vocoder, 0, v2s, 0);
	connect(v2s, 0, self(), 0);
}
AudioSource::sptr AudioSource::Create(unsigned int _freq)
{
	return gnuradio::get_initial_sptr(new AudioSource(_freq));
}
AudioSink::AudioSink(unsigned int _freq) :
	gr::hier_block2("AudioSink", gr::io_signature::make(1, 1, sizeof(unsigned char)), gr::io_signature::make(0, 0, 0))
{
	VocoderDecoder::sptr vocoder = VocoderDecoder::Create();
	gr_stream_to_vector_sptr s2v = gr_make_stream_to_vector(sizeof(unsigned char), 33);
	connect(self(), 0, s2v, 0);
	connect(s2v, 0, vocoder, 0);
	connect(vocoder, 0, audio_make_sink(6100, "", false), 0);
}
AudioSink::sptr AudioSink::Create(unsigned int _freq)
{
	return gnuradio::get_initial_sptr(new AudioSink(_freq));
}
