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

#ifndef _INC_BBVLC_FRAME_GENERATOR_H
#define _INC_BBVLC_FRAME_GENERATOR_H

#include <gnuradio/block.h>

/*! \brief bbVLC_Frame_Generator is the block which adds the FLP (Fast Locking Pattern) and the IFS (Interframing Spacing) sequences to the unit constituted by: TDP (topology dependant pattern) + PHR + PSDU units according to the system configuration.

The FLP sequence is constructed according to the section 8.6.1 of IEEE 802.15.7.
The IFS sequence is constructed according to the section 8.5.1.2 of IEEE 802.15.7. These patterns are interleaved to obtain different visibility level. 
*/

class bbVLC_Frame_Generator : public gr::block
{
	public:
		typedef boost::shared_ptr<bbVLC_Frame_Generator> sptr;
	   /**
       * The creation of the bbVLC_Frame_Generator block requires 7 parameters: 
       * @param  _d_mode: to indicate whether OOK or VPPM is being used 
       * @param  _FLP_length: length in bits of the Fast Locking Pattern.
       * @param  _tx_mode: the transmission operating mode (single, packed, burst) defines the length of the interframing sequence length. 
       * @param  _PSDU_units: the number of PSDU_units in the frame. Example, in the single transmission mode this value is always one. 
       * @param  _length_PHR : length of the encoded PHR block of data 
       * @param  _length_data_payload: length of the encoded PSDU unit of data.
       * @param  _length_burst: the space in frames between the repetition of the FLP pattern
       */
		static sptr Create(int,int,int,int,int,int,int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		~bbVLC_Frame_Generator();
	
	private:
		bbVLC_Frame_Generator(int, int,int,int,int,int,int);
		int FLP_length;
		int tx_mode;
		int PSDU_units;
		float *FLP_pattern;
		float *idle_pattern;
		int IFS;
		int length_PHR; //length of modulated PHR
		int length_data_payload; // length of modulated data
		int length_frame;
		int FLP_counter; //to control the inclusion of the FLP preamble in the burst mode
		int length_burst;
		int d_mode;
};

#endif //_INC_BBVLC_FRAME_GENERATOR_H
