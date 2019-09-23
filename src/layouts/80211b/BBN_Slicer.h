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

#ifndef _INC_BBN_SLICER_H_
#define _INC_BBN_SLICER_H_

#include <gnuradio/block.h>

class BBN_Slicer : public gr::block
{
	public:
		typedef boost::shared_ptr<BBN_Slicer> sptr;
		static sptr Create(int, int);
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		void forecast(int, gr_vector_int &);
		~BBN_Slicer();
	
	private:
		BBN_Slicer(int, int);
		int d_samples_per_symbol;
		int d_sample_block_size;
		int d_symbol_index;
		int d_offset;
		float d_f_offset;
		float d_f_samples_per_symbol;
		float d_gain;
		float *d_sums;
};

#endif //_INC_BBN_SLICER_H_
