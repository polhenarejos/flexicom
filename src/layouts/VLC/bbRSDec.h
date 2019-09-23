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

#ifndef _INC_BBRSDEC_H
#define _INC_BBRSDEC_H

#include <gnuradio/block.h>
#include "vlc_reed_solomon.h"

/*! \brief bbRSDec is the block in charge to perform the Reed Solomon decoding according to the input parameters 

*/

class bbRSDec : public gr::block
{
	public:
		typedef boost::shared_ptr<bbRSDec> sptr;
	   /**
       * The creation of the bbRSDec block requires 5 parameters: 
       * @param _GF: depth in bits of the Galois Field used in the Reed Solomon encoding process. (PHY I=4 bits, PHYII=8 bits)
       * @param _N: the output number of elements of an encoded word.
       * @param _K: the minimum number of elements to obtain a N-element encoded word.
       * @param _phy_type: distinguish between PHY operating modes (PHY I-PHY II) in order to generate the proper polynomial
       * @param _length: number of elements in a block of data to be decoded coming from the deinterleaver
       */
		static sptr Create(unsigned int, unsigned int, unsigned int, int, int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		int out_rs_dec; /**< [out] indicates the length in bits of the output of the bbRSDec block*/
		~bbRSDec();
		static void Decode(const int *, int *, int, int, int, int, int, int, int, vlc_reed_solomon *);
		static int OutRS(int, int, int, int);
	
	private:
		bbRSDec(unsigned int, unsigned int, unsigned int, int, int);
		unsigned int GF; //dimension of the galois Field in bits PHY I=4 bits, PHYII=8 bits
		unsigned int N; //length of the output block
		unsigned int K; //length of the input block
		int phy_type;
		int pre_length; //length in units of the input data from the previous block (deinterleaver)
		vlc_reed_solomon *vlc_rs;
};

#endif //_INC_BBRSDEC_H
