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
