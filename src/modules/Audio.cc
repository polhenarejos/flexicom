// $Id: InterPunct.cc 181 2012-08-22 13:35:18Z phenarejos $

#include "Audio.h"
#include <gr_float_to_short.h>
#include <gr_short_to_float.h>
#include <vocoder_gsm_fr_encode_sp.h>
#include <vocoder_gsm_fr_decode_ps.h>
#include <gr_vector_to_stream.h>
#include <gr_stream_to_vector.h>
#include <gr_multiply_const_ff.h>
#include <gr_sig_source_f.h>
#include <gr_audio_source.h>
#include <gr_audio_sink.h>
#include <gr_io_signature.h>

AudioSource::AudioSource(unsigned int _freq) :
	gr_hier_block2("AudioSource", gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, 1, sizeof(unsigned char)))
{
	gr_float_to_short_sptr f2s = gr_make_float_to_short();
	vocoder_gsm_fr_encode_sp_sptr vocoder = vocoder_make_gsm_fr_encode_sp();
	gr_vector_to_stream_sptr v2s = gr_make_vector_to_stream(sizeof(unsigned char), 33);
	gr_multiply_const_ff_sptr mc = gr_make_multiply_const_ff(32767);
	//connect(gr_make_sig_source_f(8000, GR_SIN_WAVE, 350, 0.1), 0, mc, 0);
	connect(audio_make_source(8000), 0, mc, 0);
	connect(mc, 0, f2s, 0);
	connect(f2s, 0, vocoder, 0);
	connect(vocoder, 0, v2s, 0);
	connect(v2s, 0, self(), 0);
}
AudioSource::sptr AudioSource::Create(unsigned int _freq)
{
	return gnuradio::get_initial_sptr(new AudioSource(_freq));
}
AudioSink::AudioSink(unsigned int _freq) :
	gr_hier_block2("AudioSink", gr_make_io_signature(1, 1, sizeof(unsigned char)), gr_make_io_signature(0, 0, 0))
{
	gr_stream_to_vector_sptr s2v = gr_make_stream_to_vector(sizeof(unsigned char), 33);
	vocoder_gsm_fr_decode_ps_sptr vocoder = vocoder_make_gsm_fr_decode_ps();
	gr_short_to_float_sptr s2f = gr_make_short_to_float();
	gr_multiply_const_ff_sptr mc = gr_make_multiply_const_ff(1./32767.);
	connect(self(), 0, s2v, 0);
	connect(s2v, 0, vocoder, 0);
	connect(vocoder, 0, s2f, 0);
	connect(s2f, 0, mc, 0);
	connect(mc, 0, audio_make_sink(8000, "", false), 0);
}
AudioSink::sptr AudioSink::Create(unsigned int _freq)
{
	return gnuradio::get_initial_sptr(new AudioSink(_freq));
}
