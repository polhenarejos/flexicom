#include "MSE.h"
#include <gr_sub_cc.h>
#include <gr_complex_to_xxx.h>
#include <gr_moving_average_ff.h>
#include <gr_io_signature.h>

MSE::MSE() :
	gr_hier_block2("MSE", gr_make_io_signature(2, 2, sizeof(std::complex<float>)), gr_make_io_signature(1, 1, sizeof(float)))
{
	gr_sub_cc_sptr sub = gr_make_sub_cc();
	gr_complex_to_mag_squared_sptr sqd = gr_make_complex_to_mag_squared();
	gr_moving_average_ff_sptr ma = gr_make_moving_average_ff(100, 0.01);
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
