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

#ifndef _INC_BB8B10BENC_H_
#define _INC_BB8B10BENC_H_

#include <gnuradio/block.h>

/*! \brief bb8b10bEnc class defines the RLL encoding process defined in IEEE 802.15.7.

* This scheme is used in PHY II when using OOK modulation. The 8B10B expands 8-bits to 10-bit encoded symbols with
  DC balance. This encoding process is not explicitily specified in the standard, but the following web resource has been
  selected as reference: http://www.latticesemi.com/lit/docs/refdesigns/rd1012.pdf?jsessionid=f030dc40700e304b612a7c7d361735751137
*/

class bb8b10bEnc : public gr::block
{
	public:
		typedef boost::shared_ptr<bb8b10bEnc> sptr;
		static sptr Create(int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
	
	private:
		bb8b10bEnc(int);
		int RD; //disparity indicator
		int words_processed;
		int words_in_frame;
};

#endif //_INC_BB8B10BENC_H_
