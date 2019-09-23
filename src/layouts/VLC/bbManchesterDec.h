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

#ifndef _INC_BBMANCHESTERDEC_H_
#define _INC_BBMANCHESTERDEC_H_

#include <gnuradio/sync_decimator.h>

/*! \brief bbManchesterDec performs the Manchester decoding of the operation specified in section 10.5.2 of IEEE 802.15.7

*  The Manchester decoding works in a 2-bits basis symbol as shown in Table 103 to return one bit.
Used with the OOK modulation in PHY I operating mode.
*/

class bbManchesterDec : public gr::sync_decimator
{
	public:
		typedef boost::shared_ptr<bbManchesterDec> sptr;
		/**
       * The creation of the bbManchesterDec requires 2 parameters: 
       * @param mode: selects in which mode works the block. Two options: 0 and 1. 0 is for OOK and 1 is used with VPPM with 50% dimming (assuming double clock frequency than in OOK)
       * @param flag_cc: indicates whether the next block in the chain will be a CC decoder. In the case of CC decoding, the signal at the input of this block must be adapted.
       */
		static sptr Create(int,int);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		~bbManchesterDec();
		static void Decode(const float *, int *, int, int, int);
	
	private:
		bbManchesterDec(int,int);
		int d_mode;
		int d_flag_cc; //1: after manchester, there is the cc decoder
};

#endif //_INC_BBMANCHESTERDEC_H_
