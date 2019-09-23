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

#ifndef _INC_BB4B6BDEC_H_
#define _INC_BB4B6BDEC_H_

#include <gnuradio/block.h>

/*! \brief bb4b6bDec class defines the RLL decoding operation of the process defined in section 10.5.1 of IEEE 802.15.7.

* All VPPM PHY I modes shall use 4B6B encoding. The 4B6B decoder takes 6-bit encoded symbols with
  DC balance and returns its 4-bit representation. The decoding process is based in a correlation-distance measurement.
*/

class bb4b6bDec : public gr::block
{
	public:
		typedef boost::shared_ptr<bb4b6bDec> sptr;
		static sptr Create();
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		static int distance(int *);
		static const int table[];
		
		static void Decode(const int *, int *, int);
		
	private:
		bb4b6bDec();
};

#endif //_INC_BB4B6BDEC_H_
