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

#ifndef _INC_BBRSENC_H
#define _INC_BBRSENC_H

#include <gnuradio/block.h>
#include "vlc_reed_solomon.h"

/*! \brief bbRSEnc is the block in charge to perform the Reed Solomon encoding according to the sections 10.2 and 11.2 of IEEE 802.15.7

Systematic RS codes are used for the PHY I outer FEC with GF(16), generated by the polynomial x4+x+1.
The generators for the RS(n, k) codes for PHY I (see Table 73) are given in Table 101 
A systematic Reed-Solomon code operating on GF(256) shall be used for PHY II. The Reed-Solomon code is defined over GF(256) with a primitive polynomial x8+x4+x3+x2+1.

*/

class bbRSEnc : public gr::block
{
	public:
		typedef boost::shared_ptr<bbRSEnc> sptr;
	   /**
       * The creation of the bbRSEnc block requires 5 parameters: 
       * @param _GF: depth in bits of the Galois Field used in the Reed Solomon encoding process. (PHY I=4 bits, PHYII=8 bits)
       * @param _N: the output number of elements of an encoded word.
       * @param _K: the minimum number of elements to obtain a N-element encoded word.
       * @param _phy_type: distinguish between PHY operating modes (PHY I-PHY II) in order to generate the proper polynomial
       * @param _length: number of elements in bits in a block of data to be encoded.
       */
		static sptr Create(unsigned int *, unsigned int *, unsigned int *, unsigned int *, unsigned int *);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		int out_rs; /**< [out] indicates the length in bits of the output of the bbRSEnc block*/
		~bbRSEnc();
		static int OutRS(int, int, int, int, int);
		static int Encode(const int *, int *, int, int, int, int, int, int, int, vlc_reed_solomon *);
	
	private:
		bbRSEnc(unsigned int *, unsigned int *, unsigned int *, unsigned int *, unsigned int *);
		void ctor();
		void ctor(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
		unsigned int *pGF, *pN, *pK, *pphy_type, *plength;
		unsigned int GF; //dimension of the galois Field in bits PHY I=4 bits, PHYII=8 bits
		unsigned int N; //length of the output block
		unsigned int K; //length of the input block
		unsigned int phy_type;
		unsigned int length; //length in bits of the input data
		vlc_reed_solomon *vlc_rs;
		int rs_out_elements();
};

#endif //_INC_BBRSENC_H
