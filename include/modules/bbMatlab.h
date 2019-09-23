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

#ifndef _INC_BBMATLAB_H_
#define _INC_BBMATLAB_H_

#include <gnuradio/sync_block.h>
#include <complex>

/*! \brief bbMatlab class is used to extract data from the system chain to post-process it in Matlab
*/
class bbMatlab : public gr::sync_block
{
	public:
		typedef boost::shared_ptr<bbMatlab> sptr;
	   /**
       * The creation of bbMatlab requires 2 parameters: 
       * @param f: the name of the file where the samples are going to be stored.
       * @param s: the size of the data to be stored. 3 possible values: float, double (float and double format).
       */
		static sptr Create(std::string, size_t=sizeof(gr_complex));
		int work(int, gr_vector_const_void_star&, gr_vector_void_star&);
		~bbMatlab();
	
	private:
		bbMatlab(std::string, size_t = sizeof(gr_complex));
		FILE *fp;
		size_t s;
		int cplx;

};

#endif //_INC_BBMATLAB_H_
