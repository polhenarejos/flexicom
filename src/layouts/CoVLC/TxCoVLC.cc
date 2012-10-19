// $Id$

#include "TxCoVLC.h"
#include "LayoutCoVLC.h"
#include <gr_io_signature.h>
#include <gr_vector_source_b.h>
#include <gr_fft_vcc.h>
#include <vector>
#include <gr_unpacked_to_packed_bb.h>
#include <gr_packed_to_unpacked_bb.h>
#include <gr_chunks_to_symbols_bc.h>
#include <gr_vector_to_stream.h>
#include <gr_stream_to_vector.h>
#include <gr_multiply_const_cc.h>
#include <digital_ofdm_cyclic_prefixer.h>
#include <gr_complex_to_xxx.h>
#include <gr_float_to_complex.h>
#include <gr_multiply_ff.h>
#include <gr_sub_ff.h>
#include <gr_sig_source_f.h>
#include <filter/firdes.h>
#include <filter/fir_filter_ccf.h>
#include <gr_keep_m_in_n.h>
#include <gr_null_source.h>
#include <gr_stream_mux.h>
#include "../../modules/Oversampler.cc"
#include "bbMatlab.h"

#define ISQRT2 0.7071067811865475

TxCoVLC::TxCoVLC(LayoutCoVLC * _ly) :
	gr_hier_block2("TxCoVLC", gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, 1, sizeof(gr_complex))),
	ly(_ly)
{
	unsigned char _v[] = { 
					0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,1,1,0,0,0,1,0,0,0,1,1,0,
					1,1,0,1,0,1,1,1,1,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,0,0,0,1,0,0,1,0,
					0,0,1,1,1,0,0,1,0,1,1,1,0,1,0,0,0,1,0,0,1,0,0,0,1,0,1,1,1,1,0,0,
					1,0,1,0,1,0,0,1,0,1,0,0,0,0,1,0,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0 };
	std::vector<unsigned char> v(_v, _v+sizeof(_v)/sizeof(unsigned char));
	std::vector<float> f(0);
	gr_vector_source_b_sptr ds = gr_make_vector_source_b(v, false);
	gr_unpacked_to_packed_bb_sptr pck = gr_make_unpacked_to_packed_bb(1, GR_MSB_FIRST);
	gr_packed_to_unpacked_bb_sptr upck = gr_make_packed_to_unpacked_bb(2, GR_MSB_FIRST);
	gr_complex _tbl[] = { gr_complex(ISQRT2,ISQRT2), gr_complex(ISQRT2, -ISQRT2), gr_complex(-ISQRT2, ISQRT2), gr_complex(-ISQRT2,-ISQRT2) };
	std::vector<gr_complex> tbl(_tbl, _tbl+sizeof(_tbl)/sizeof(gr_complex));
	gr_fft_vcc_sptr fft = gr_make_fft_vcc(64, false, f, true, 2);
	gr_chunks_to_symbols_bc_sptr ch2sym = gr_make_chunks_to_symbols_bc(tbl);
	gr_stream_to_vector_sptr s2v = gr_make_stream_to_vector(sizeof(gr_complex), 64);
	gr_multiply_const_cc_sptr mc = gr_make_multiply_const_cc(gr_complex(1./64, 0), 64);
	digital_ofdm_cyclic_prefixer_sptr ofdm = digital_make_ofdm_cyclic_prefixer(64, 64+16);
	gr_complex_to_float_sptr c2f = gr_make_complex_to_float();
	gr_sig_source_f_sptr cos = gr_make_sig_source_f(2000e3, GR_COS_WAVE, 500e3, 1., 0.);
	gr_sig_source_f_sptr sin = gr_make_sig_source_f(2000e3, GR_SIN_WAVE, 500e3, 1., 0.);
	gr_multiply_ff_sptr mulI = gr_make_multiply_ff(), mulQ = gr_make_multiply_ff();
	gr_sub_ff_sptr sub = gr_make_sub_ff();
	std::vector<float> taps = gr::filter::firdes::low_pass_2(1, 2000e3, 200e3, 200e3, 90);
	gr_null_source_sptr nls = gr_make_null_source(sizeof(gr_complex));
	std::vector<int> lens(2); lens[0] = 400; lens[1] = taps.size();
	gr_stream_mux_sptr mux = gr_make_stream_mux(sizeof(gr_complex), lens);
	gr_keep_m_in_n_sptr filt_delay = gr_make_keep_m_in_n(sizeof(gr_complex), 400, 400+taps.size(), taps.size());
	gr::filter::fir_filter_ccf::sptr filter = gr::filter::fir_filter_ccf::make(1, taps);
	gr_float_to_complex_sptr f2c = gr_make_float_to_complex();
	Oversampler<gr_complex>::sptr ov = Oversampler<gr_complex>::Create(5);
	bbMatlab::sptr bbm = bbMatlab::Create("Flexicom2", sizeof(gr_complex));
	connect(ds, 0, pck, 0);
	connect(pck, 0, upck, 0);
	connect(upck, 0, ch2sym, 0);
	connect(ch2sym, 0, s2v, 0);
	connect(s2v, 0, fft, 0);
	connect(fft, 0, mc, 0);
	connect(mc, 0, ofdm, 0);
	connect(ofdm, 0, ov, 0);
	connect(ov, 0, mux, 0);
	connect(nls, 0, mux, 1);
	connect(mux, 0, filter, 0);
	connect(filter, 0, filt_delay, 0);
	connect(filt_delay, 0, c2f, 0);
	connect(c2f, 0, mulI, 0);
	connect(cos, 0, mulI, 1);
	connect(c2f, 1, mulQ, 0);
	connect(sin, 0, mulQ, 1);
	connect(mulI, 0, sub, 0);
	connect(mulQ, 0, sub, 1);
	connect(sub, 0, f2c, 0);
	connect(f2c, 0, self(), 0);
	connect(f2c, 0 , bbm, 0);
}
TxCoVLC::sptr TxCoVLC::Create(LayoutCoVLC * _ly)
{
	return gnuradio::get_initial_sptr(new TxCoVLC(_ly));
}
