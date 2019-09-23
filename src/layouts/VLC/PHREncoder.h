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

#ifndef _INC_PHRENCODER_H
#define _INC_PHRENCODER_H

#include <gnuradio/block.h>
#include "LayoutVLC.h"
class vlc_convolutional_coding;
class vlc_reed_solomon;

class PHREncoder : public gr::block
{
	public:
		typedef boost::shared_ptr<PHREncoder> sptr;
		static sptr Create();
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		~PHREncoder();
	
	private:
		PHREncoder();
		int cpd;
		int *buf, *b;
		int *phrbuf;
		void SetBuffer(int, LayoutVLC::PHYType, LayoutVLC::Modulation, int);
		int *ienc, *ibi, *iilv, *ipu, *irs, *iphr;
		int ProcessPHR();
		PHYHdr ph;
		LayoutVLC::PHYType phy_type;
		LayoutVLC::Modulation mod;
		int rate;
		std::vector<std::vector<int> > payloads;
		vlc_convolutional_coding *CC;
		vlc_reed_solomon *RS[2];
		std::vector<unsigned int> ivector[2];
		static const int _TDP[];
		const int *TDP[2];
		int flen;
};

#endif //_INC_PHRENCODER_H
