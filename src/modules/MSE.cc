// $Id$
#include "MSE.h"
#include <gnuradio/blocks/sub.h>
#include <gnuradio/blocks/complex_to_mag_squared.h>
#include <gnuradio/blocks/moving_average.h>
#include <gnuradio/io_signature.h>

MSE::MSE() :
	gr::hier_block2("MSE", gr::io_signature::make(2, 2, sizeof(std::complex<float>)), gr::io_signature::make(1, 1, sizeof(float)))
{
	gr::blocks::sub_cc::sptr sub = gr::blocks::sub_cc::make();
	gr::blocks::complex_to_mag_squared::sptr sqd = gr::blocks::complex_to_mag_squared::make();
	gr::blocks::moving_average_ff::sptr ma = gr::blocks::moving_average_ff::make(100, 0.01);
	connect(self(), 0, sub, 0);
	connect(self(), 1, sub, 1);
	connect(sub, 0, sqd, 0);
	connect(sqd, 0, ma, 0);
	connect(ma, 0, self(), 0);
}
MSE::sptr MSE::Create()
{
	return gnuradio::get_initial_sptr(new MSE());
}
