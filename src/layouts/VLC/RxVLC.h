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

#ifndef _INC_RXVLC_H_
#define _INC_RXVLC_H_

#include <gnuradio/hier_block2.h>
#include <gnuradio/msg_queue.h>

class LayoutVLC;

/*! \brief The class where the layout of the VLC communication system (receiver part) is defined

* In this class, the TXVLC or the RXVLC is connected to the usrp class object to stablish the stream of samples
* Another functionality of this class is to read the tab which configures the VLC system.

*/

class RxVLC : public gr::hier_block2
{
	public:
		typedef boost::shared_ptr<RxVLC> sptr;
		static sptr Create(LayoutVLC *);
		~RxVLC();
		void stop();
		
	private:
		int poly[3];
		RxVLC(LayoutVLC *);
		LayoutVLC *ly;
		void init_var();
		unsigned int PHR_modulated_length; //length of the PHR after all the modulating chain
		unsigned int PSDU_modulated_length; //length of the PSDU after all the modulating chain
};

#endif //_INC_RXVLC_H_
