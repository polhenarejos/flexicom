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

#ifndef _INC_PSDUENCODER_H
#define _INC_PSDUENCODER_H

#include <gnuradio/block.h>
#include "LayoutVLC.h"

class PSDUEncoder : public gr::block
{
	public:
		typedef boost::shared_ptr<PSDUEncoder> sptr;
		static sptr Create();
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
	
	private:
		PSDUEncoder();
		int cpd;
		int *buf, *b;
		void SetBuffer(int, LayoutVLC::PHYType, LayoutVLC::Modulation, int);
		int *ienc, *ibi, *iilv, *ipu, *irs, *ipld;
		int ProcessPSDU();
		PHYHdr ph;
		LayoutVLC::PHYType phy_type;
		LayoutVLC::Modulation mod;
		int rate;
		std::vector<std::vector<int> > payloads;
		std::vector<PHYHdr> phys;
		void SetMHR();
		unsigned char seq_num;
};

#endif //_INC_PSDUENCODER_H
