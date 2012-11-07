// $Id$

#include "RxCoVLC.h"
#include "LayoutCoVLC.h"
#include <gr_io_signature.h>
#include <gr_vector_source_b.h>
#include <gr_fft_vcc.h>
#include <vector>
#include <gr_packed_to_unpacked_bb.h>
#include <gr_vector_to_stream.h>
#include <gr_stream_to_vector.h>
#include <gr_multiply_const_cc.h>
#include <gr_complex_to_xxx.h>
#include <gr_float_to_complex.h>
#include <gr_multiply_ff.h>
#include <gr_sig_source_f.h>
#include <filter/fir_filter_ccf.h>
#include <gr_keep_m_in_n.h>
#include <gr_null_source.h>
#include <gr_stream_mux.h>
#include "bbMatlab.h"
#include <gr_null_sink.h>
#include "gr_symbols_to_chunks_cb.h"
#include "OFDMSync.h"
#include <gr_noise_source_c.h>
#include <gr_add_cc.h>

#define ISQRT2 0.7071067811865475

RxCoVLC::RxCoVLC(LayoutCoVLC * _ly) :
	gr_hier_block2("RxCoVLC", gr_make_io_signature(1, 1, sizeof(gr_complex)), gr_make_io_signature(0, 0, 0)),
	ly(_ly)
{
	std::vector<float> f(0);
	gr_complex_to_float_sptr c2f = gr_make_complex_to_float();
	gr_sig_source_f_sptr cos = gr_make_sig_source_f(2000e3, GR_COS_WAVE, 500e3, 1., 0.);
	gr_sig_source_f_sptr sin = gr_make_sig_source_f(2000e3, GR_SIN_WAVE, 500e3, -1., 0.);
	gr_multiply_ff_sptr mulI = gr_make_multiply_ff(), mulQ = gr_make_multiply_ff();
	gr_float_to_complex_sptr f2c = gr_make_float_to_complex();
	std::vector<float> taps(2);
	taps[0] = 0.5; taps[1] = 0.5;
	gr_keep_m_in_n_sptr filt_delay = gr_make_keep_m_in_n(sizeof(gr_complex), 400, 400+taps.size(), 0);
	gr_null_source_sptr nls = gr_make_null_source(sizeof(gr_complex));
	std::vector<int> lens(2); lens[0] = 400; lens[1] = taps.size();
	gr_stream_mux_sptr mux = gr_make_stream_mux(sizeof(gr_complex), lens);
	gr::filter::fir_filter_ccf::sptr filter = gr::filter::fir_filter_ccf::make(1, taps);
	gr_keep_m_in_n_sptr decim =  gr_make_keep_m_in_n(sizeof(gr_complex), 1, 5, 0);
	gr_keep_m_in_n_sptr cyc = gr_make_keep_m_in_n(sizeof(gr_complex), 64, 80, 16);
	gr_stream_to_vector_sptr s2v = gr_make_stream_to_vector(sizeof(gr_complex), 64);
	gr_vector_to_stream_sptr v2s = gr_make_vector_to_stream(sizeof(gr_complex), 64);
	gr_fft_vcc_sptr fft = gr_make_fft_vcc(64, true, f, true, 1);
	bbMatlab::sptr bbm = bbMatlab::Create("Flexicom", sizeof(unsigned char));
	gr_null_sink_sptr nl = gr_make_null_sink(sizeof(unsigned char));
	gr_multiply_const_cc_sptr mc = gr_make_multiply_const_cc(gr_complex(2, 0));
	gr_complex _tbl[] = { gr_complex(ISQRT2,ISQRT2), gr_complex(ISQRT2, -ISQRT2), gr_complex(-ISQRT2, ISQRT2), gr_complex(-ISQRT2,-ISQRT2) };
	std::vector<gr_complex> tbl(_tbl, _tbl+sizeof(_tbl)/sizeof(gr_complex));
	gr_symbols_to_chunks_cb_sptr sym2ch = gr_make_symbols_to_chunks_cb(tbl);
	gr_packed_to_unpacked_bb_sptr upck = gr_make_packed_to_unpacked_bb(1, GR_MSB_FIRST);
	gr_keep_m_in_n_sptr tupck = gr_make_keep_m_in_n(sizeof(unsigned char), 2, 8, 6);
	gr_keep_m_in_n_sptr cfreq = gr_make_keep_m_in_n(sizeof(gr_complex), 56, 64, 4);
	OFDMSync::sptr sync = OFDMSync::Create(64, 16);
	//gr_noise_source_c_sptr noise = gr_make_noise_source_c(GR_GAUSSIAN, 0.01, -238497);
	gr_null_source_sptr noise = gr_make_null_source(sizeof(gr_complex));
	gr_add_cc_sptr addn = gr_make_add_cc();
	connect(self(), 0, addn, 0);
	connect(noise, 0, addn, 1);
	connect(addn, 0, c2f, 0);
	connect(c2f, 0, mulI, 0);
	connect(cos, 0, mulI, 1);
	connect(c2f, 0, mulQ, 0);
	connect(sin, 0, mulQ, 1);
	connect(mulI, 0, f2c, 0);
	connect(mulQ, 0, f2c, 1);
	connect(f2c, 0, mux, 0);
	connect(nls, 0, mux, 1);
	connect(mux, 0, filter, 0);
	connect(filter, 0, filt_delay, 0);
	connect(filt_delay, 0, decim, 0);
	connect(decim, 0, sync, 0);
	connect(sync, 0, cyc, 0);
	connect(cyc, 0, mc, 0);
	connect(mc, 0, s2v, 0);
	connect(s2v, 0, fft, 0);
	connect(fft, 0, v2s, 0);
	connect(v2s, 0, cfreq, 0);
	connect(cfreq, 0, sym2ch, 0);
	connect(sym2ch, 0, upck, 0);
	connect(upck, 0, tupck, 0);
	connect(tupck, 0, nl, 0);
	connect(tupck, 0, bbm, 0);
}
RxCoVLC::sptr RxCoVLC::Create(LayoutCoVLC * _ly)
{
	return gnuradio::get_initial_sptr(new RxCoVLC(_ly));
}
