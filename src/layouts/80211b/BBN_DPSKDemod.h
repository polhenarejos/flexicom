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

#ifndef _INC_BBN_DPSKDEMOD_H_
#define _INC_BBN_DPSKDEMOD_H_

#include <gnuradio/block.h>

#define RSSI_AVE_COUNT 128
#define RSSI_MAX 0

class BBN_DPSKDemod : public gr::block 
{
	public:
		typedef boost::shared_ptr<BBN_DPSKDemod> sptr;
		static sptr Create();
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		void forecast(int, gr_vector_int &);
		
	private:
		BBN_DPSKDemod();
		void UpdateRSSI();
		void InitLogTable();
		float d_e_squared;
		int d_sample_count;
		short d_rssi;
		gr_complex d_prev;
		char d_log_table[64];
};

#endif //_INC_BBN_DPSKDEMOD_H_
