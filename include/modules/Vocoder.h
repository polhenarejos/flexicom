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

#ifndef _INC_VOCODER_H_
#define _INC_VOCODER_H_

#include <gnuradio/sync_decimator.h>
#include <gnuradio/sync_interpolator.h>

class VocoderEncoder : public gr::sync_decimator 
{
	public:
		typedef boost::shared_ptr<VocoderEncoder> sptr;
		static sptr Create();
		~VocoderEncoder();
		
	private:
		VocoderEncoder();
		int work (int, gr_vector_const_void_star &, gr_vector_void_star &);
		struct gsm_state *dgsm;
		int reset;
};

class VocoderDecoder : public gr::sync_interpolator
{
	public:
		typedef boost::shared_ptr<VocoderDecoder> sptr;
		static sptr Create();
		~VocoderDecoder();
		
	private:
		VocoderDecoder();
		int work (int, gr_vector_const_void_star &, gr_vector_void_star &);
		struct gsm_state *dgsm;
};

#endif //_INC_VOCODER_H_
