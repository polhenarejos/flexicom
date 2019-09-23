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

#ifndef _INC_PSDUDECODER_H
#define _INC_PSDUDECODER_H

#include <gnuradio/block.h>
#include "LayoutVLC.h"

class PSDUDecoder : public gr::block
{
	public:
		typedef boost::shared_ptr<PSDUDecoder> sptr;
		static sptr Create(LayoutVLC *);
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
	
	private:
		PSDUDecoder(LayoutVLC *);
		int cpd;
		float *buf, *b;
		int *buf_bis;
		void SetBuffer(int, LayoutVLC::PHYType, LayoutVLC::Modulation, int);
		int *idec, *ibi, *iilv, *irs, *ipld;
		int ProcessPSDU();
		PHYHdr ph;
		LayoutVLC::PHYType phy_type;
		LayoutVLC::Modulation mod;
		int rate;
		std::vector<std::vector<unsigned char> > payloads;
		std::vector<PHYHdr> phys;
		unsigned int CRCok, CRCnok;
		LayoutVLC *ly;
};

#endif //_INC_PSDUDECODER_H
