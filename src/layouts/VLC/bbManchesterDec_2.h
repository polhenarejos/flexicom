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

#ifndef _INC_BBMANCHESTERDEC_2_H_
#define _INC_BBMANCHESTERDEC_2_H_

#include <gnuradio/sync_decimator.h>

/*! \brief BBMANCHESTERDEC_2 performs the Manchester decoding of the operation specified in section 10.5.2 of IEEE 802.15.7

*  The Manchester decoding works in a 2-bits basis symbol as shown in Table 103 to return one bit.
Used with the OOK modulation in PHY I operating mode.
*/

class bbManchesterDec_2 : public gr::sync_decimator
{
	public:
		typedef boost::shared_ptr<bbManchesterDec_2> sptr;
		/**
       * The creation of the bbManchesterDec_2 does not require any parameter. This a special decoder for the case of having convolutional decoding 1/4, to take profit of the 
       * introduced redundancy. This block has a decimation factor of 4.
       */
		static sptr Create();
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		~bbManchesterDec_2();
		static void Decode(const float *, int *, int);
	
	private:
		bbManchesterDec_2();
};

#endif //_INC_BBMANCHESTERDEC_2_H_
