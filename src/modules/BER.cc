// $Id: Timing.cc 251 2012-09-07 23:12:26Z phenarejos $

#include "BER.h"
#include <gr_io_signature.h>
#include <volk/volk.h>
#include <gnuradio/malloc16.h>

BER::BER() :
	gr_sync_block("BER", gr_make_io_signature(2, 2, sizeof(int)), gr_make_io_signature(1, 1, sizeof(int))),
	bits(0), diff(0)
{
}
BER::sptr BER::Create()
{
	return sptr(new BER());
}
int BER::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	const int *iptr0 = (const int *)_i[0], *iptr1 = (const int *)_i[1];
	int *optr = (int *)_o[0];
	float *E0 = (float *)malloc16Align(sizeof(float)*no), *E1 = (float *)malloc16Align(sizeof(float)*no);
	if (is_unaligned())
	{
		volk_32i_s32f_convert_32f_u(E0, iptr0, 1.0, no);
		volk_32i_s32f_convert_32f_u(E1, iptr1, 1.0, no);
	}
	else
	{
		volk_32i_s32f_convert_32f_a(E0, iptr0, 1.0, no);
		volk_32i_s32f_convert_32f_a(E1, iptr1, 1.0, no);
	}
	volk_32f_x2_subtract_32f_a(E0, E0, E1, no);
	volk_32f_x2_multiply_32f_a(E0, E0, E0, no);
	volk_32f_sqrt_32f_a(E0, E0, no);
	float d = 0;
	volk_32f_accumulator_s32f_a(&d, E0, no);
	diff += d;
	bits += no;
	add_item_tag(0, nitems_written(0), pmt::pmt_string_to_symbol("BER"), pmt::pmt_from_double((double)diff/bits), pmt::pmt_string_to_symbol(name()));
	memcpy(optr, iptr0, sizeof(int)*no);
	free16Align(E0);
	free16Align(E1);
	return no;
}
