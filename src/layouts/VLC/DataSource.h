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

#ifndef _INC_DATASOURCE_H_
#define _INC_DATASOURCE_H_

#include <gnuradio/block.h>

class LayoutVLC;
class DataSource : public gr::block
{
	public:
		typedef boost::shared_ptr<DataSource> sptr;
		static sptr Create(int, LayoutVLC *, bool);
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		int PushData(unsigned char *, unsigned short, int = 0);
		typedef struct
		{
			unsigned short size;
			unsigned char *data;
			int free_method; //0 -> no free, 1 -> free(), 2 -> delete []
		}Data;
		
	private:
		DataSource(int, LayoutVLC *, bool = true);
		int len;
		int ic;
		bool pend;
		int databyte[8];
		std::vector<Data> data;
		bool voip;
		int dataoff;
		bool prevreset;
		LayoutVLC *ly;
};

#endif //_INC_DATASOURCE_H_
