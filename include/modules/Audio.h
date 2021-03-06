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

#ifndef _INC_AUDIO_H_
#define _INC_AUDIO_H_

#include <gnuradio/hier_block2.h>

class AudioSource : public gr::hier_block2
{
	public:
		typedef boost::shared_ptr<AudioSource> sptr;
		static sptr Create(unsigned int);
	
	private:
		AudioSource(unsigned int);
};

class AudioSink : public gr::hier_block2
{
	public:
		typedef boost::shared_ptr<AudioSink> sptr;
		static sptr Create(unsigned int);
	
	private:
		AudioSink(unsigned int);
};

#endif //_INC_AUDIOH_
