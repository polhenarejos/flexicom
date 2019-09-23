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

#ifndef _INC_INTERLEAVER_H_
#define _INC_INTERLEAVER_H_

#include <gnuradio/sync_block.h>
#include <vector>

class Interleaver : public gr::sync_block
{
	public:
		typedef enum { INTERLEAVING, DEINTERLEAVING } Mode;
		typedef boost::shared_ptr<Interleaver> sptr;
		static sptr Create(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, Mode);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		static void Decode(const int *, int *, int, int, std::vector<unsigned int> &, Mode);
		
	private:
		Interleaver(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, Mode);
		unsigned int GF; //number of bits of each word
		unsigned int N; //the length of the output reed-solomon codeword
		unsigned int K; //the length of the input reed-solomon codeword
		unsigned int raw_length;
		unsigned int rs_length;
		std::vector<unsigned int> ivector;
		Mode mode;
};

#endif //_INC_INTERLEAVER_H_
