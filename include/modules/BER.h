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

#ifndef _INC_BER_H_
#define _INC_BER_H_

#include <gnuradio/sync_block.h>

class BER : public gr::sync_block
{
	public:
		typedef boost::shared_ptr<BER> sptr;
		static sptr Create(size_t, unsigned int, unsigned int, unsigned int, unsigned int = 0);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		
	private:
		BER(size_t, unsigned int, unsigned int, unsigned int, unsigned int = 0);
		unsigned int bits;
		unsigned int diff;
		size_t s;
		unsigned int bps;
		unsigned int sps;
		unsigned int chunk;
		unsigned int off;
		unsigned int pos;
};

#endif //_INC_BER_H
