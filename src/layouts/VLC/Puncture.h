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

#ifndef _INC_PUNCTURE_H_
#define _INC_PUNCTURE_H_

#include <gnuradio/block.h>
#include <vector>

class Puncture : public gr::block
{
	public:
		typedef boost::shared_ptr<Puncture> sptr;
		static sptr Create(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		unsigned int out;
		void forecast(int, gr_vector_int &);
		static int OutPunct(int, int, int, int);
		static int Encode(const int *, int *, int, int, std::vector<unsigned int> &, unsigned int &);
		
	private:
		Puncture(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
		unsigned int rs_length;
		std::vector<unsigned int> pvector;
		unsigned int ic;
};


#endif _INC_PUNCTURE_H_
