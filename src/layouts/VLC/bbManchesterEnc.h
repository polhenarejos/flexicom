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

#ifndef _INC_BBMANCHESTERENC_H_
#define _INC_BBMANCHESTERENC_H_

#include <gnuradio/sync_interpolator.h>

/*! \brief bbManchesterEnc performs the Manchester encoding as specified in section 10.5.2 of IEEE 802.15.7

*  The Manchester code expands each bit into an encoded 2-bit symbol as shown in Table 103. Used with the OOK modulation in PHY I operating mode.
*/
class bbManchesterEnc : public gr::sync_interpolator
{
	public:
		typedef enum { OOK = 0, VPPM = 1 } ModType;
		typedef boost::shared_ptr<bbManchesterEnc> sptr;
		/**
       * The creation of the bbManchesterEnc requires 1 parameters: 
       * @param mode: selects in which mode works the block. Two options: 0 and 1. 0 is for OOK and 1 is used with VPPM with 50% dimming (assuming double clock frequency than in OOK)
       */
		static sptr Create(ModType);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		static void Encode(const int *, int *, int, int);
	
	private:
		bbManchesterEnc(ModType);
		int d_mode;
		int d_length;
};

#endif //_INC_BBMANCHESTERENC_H_
