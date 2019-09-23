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

#ifndef _INC_BBCCDEC_H
#define _INC_BBCCDEC_H

#include <gnuradio/block.h>
#include "vlc_convolutional_coding.h"

/*! \brief bbCCDec block manages the Convolutional Decoding according to the input parameters

*  Remember, that at the transmitter size, the rate-1/3 mother convolutional code of constraint length seven (K=7) with
   generator polynomial g0 = 133; g1 = 171; g2 = 165 (octal values) has been used. 
*/

class bbCCDec : public gr::block
{
	public:
		typedef boost::shared_ptr<bbCCDec> sptr;
	   /**
       * The creation of the bbCCDec requires 5 parameters: 
       * @param _N: the rate of the convolutional code. In VLC this variable should be set to 3.
       * @param _K: the constraint length of the code. In VLC this variable should be set to 7.
       * @param _poly: the generator polynomial. In VLC this variable should be set to g0 = 133; g1 = 171; g2 = 165 (octal values)
       * @param _length: length of the input block of data to perform the convolutional decoding.
       * @param _data_rate: an index indicating the data rate selected for the modulation and the PHY operating mode selected. See Table 73 and Table 74 of IEEE 802.15.7. This value defines the puncturing scheme to be applied.
       */
		static sptr Create(int, int, int *, int, int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bbCCDec();
		int out_cc_dec; /**< [out] indicates the length in bits of the output of the bbCCDec block*/
		static int OutCC(int, int, int);
	
	private:
		bbCCDec(int, int, int *, int, int);
		int N; //mother convolutional rate
		int K; //constraint length
		int *poly;
		int length;
		int data_rate;
		vlc_convolutional_coding *vlc_cc;
};

#endif //_INC_BBCCDEC_H
