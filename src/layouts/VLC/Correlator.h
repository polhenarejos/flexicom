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

#ifndef _INC_CORRELATOR_H
#define _INC_CORRELATOR_H

#include <gnuradio/block.h>

/*! \brief Correlator is the block which receives the incoming signal from the timing recovery block in order to determine when a frame starts.
In principle, the detection mode could be made blind, but in order to save processing time, it is assumed that the topology is already known.
The output of this block is binary, indicating the beginning or not of a frame depending on the detected TDP pattern.
*/

class LayoutVLC;
class Correlator : public gr::block
{
	public:
		typedef boost::shared_ptr<Correlator> sptr;
	   /**
       * The creation of the Correlator block requires 5 parameters:
       * @param _topology: to indicate which is the sequence to do the correlation with the incoming samples.  
       * @param _d_mode: to indicate whether OOK or VPPM is being used
       */
		static sptr Create(unsigned int, LayoutVLC * = NULL, float = .75);
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		void forecast(int, gr_vector_int &);
		float *TDP[8];
		int pattern;
		~Correlator();
							  	
	private:
		Correlator(unsigned int, LayoutVLC * = NULL, float = .75);
		void Correlate(const float *, float *, float *, int, int);
		unsigned int copy;
		unsigned int cpd;
		float th;
		unsigned int ov;
		unsigned int siz;
		unsigned int vppm;
		bool strike;
		LayoutVLC *ly;
};

#endif //_INC_CORRELATOR_H
