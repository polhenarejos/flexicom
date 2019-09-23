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

// based on paper "An approximate maximum likelihood estimator for SNR jointly using pilot and data symbols,"
// Y. Chen, N. C. Beaulieu, IEEE Comm. Let., vol. 9, no. 6, 2005
 
#include "SNR.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

SNR::SNR() :
	gr::sync_block("SNR", gr::io_signature::make(1, 1, sizeof(gr_complex)), gr::io_signature::make(1, 1, sizeof(gr_complex))),
	snrs(0), times(0)
{
}
SNR::sptr SNR::Create()
{
	return sptr(new SNR());
}
#include <stdio.h>
int SNR::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	const gr_complex *iptr = (const gr_complex *)_i[0];
	gr_complex *optr = (gr_complex *)_o[0];
	if (no > 1)
	{
		float *tN = (float *)volk_malloc(sizeof(float)*no, 16);
		//if (is_unaligned())
			volk_32fc_magnitude_32f_u(tN, iptr, no);
		//else
		//	volk_32fc_magnitude_32f_a(tN, iptr, no);
		float A = 0, denA = 0;
		volk_32f_accumulator_s32f_a(&A, tN, no);
		A *= A/(no*no);
		volk_32f_x2_dot_prod_32f_a(&denA, tN, tN, no);
		//float gamma = 10*log10(A/(denA/(no-1)-no*A/(no-1)));
		//snrs += gamma;
		times++;
		float power = denA/no;
		snrs = 10.*log10((power-7e-9)/7e-9);
		add_item_tag(0, nitems_written(0), pmt::string_to_symbol("power"), pmt::from_double((double)10.*log10(denA/no)), pmt::string_to_symbol(name()));
		add_item_tag(0, nitems_written(0), pmt::string_to_symbol("snr"), pmt::from_double((double)snrs), pmt::string_to_symbol(name()));
		volk_free(tN);
	}
	memcpy(optr, iptr, sizeof(gr_complex)*no);
	return no;
}
