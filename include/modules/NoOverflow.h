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

#ifndef _INC_NOOVERFLOW_H_
#define _INC_NOOVERFLOW_H_

#include <gnuradio/sync_block.h>

class NoOverflow : public gr::sync_block
{
	public:
		typedef boost::shared_ptr<NoOverflow> sptr;
		static sptr Create(size_t, unsigned int);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		
	private:
		NoOverflow(size_t, unsigned int);
		size_t s;
		unsigned int chunk;
};

#endif //_INC_NOOVERFLOW_H
