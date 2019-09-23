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

#ifndef _INC_PHRDECODER_H_
#define _INC_PHRDECODER_H_

#include <gnuradio/sync_block.h>
#include "LayoutVLC.h"
class vlc_convolutional_coding;
class vlc_reed_solomon;

class PHRDecoder : public gr::sync_block
{
	public:
		typedef boost::shared_ptr<PHRDecoder> sptr;
		static sptr Create(LayoutVLC *);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		~PHRDecoder();
		
	private:
		PHRDecoder(LayoutVLC *);
		int *buf_bis;
		float *b, *buf;
		int cpd;
		bool ProcessPHR(PHYHdr *);
		LayoutVLC::PHYType phy_type;
		LayoutVLC::Modulation mod;
		int *idec, *ibi, *iilv, *irs, *ipld;
		vlc_convolutional_coding *CC;
		vlc_reed_solomon *RS[2];
		std::vector<unsigned int> ivector[2];
		int offPSDU; //where PHR began
		LayoutVLC *ly;
		unsigned int CRCok, CRCnok;
};

#endif //_INC_PHRDECODER_H_
