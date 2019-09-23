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

#ifndef _INC_PARSER_H_
#define _INC_PARSER_H_

#include <gnuradio/block.h>
#include "compat.h"
#include <QString>

PACK(
struct
{
	unsigned FV:2;
	unsigned rvd:4;
	unsigned FT:3;
	unsigned SE:1;
	unsigned FP:1;
	unsigned AR:1;
	unsigned DAM:2;
	unsigned SAM:2;
} , FrameControl );

PACK(
struct
{
	unsigned B:1;
	unsigned CN:3;
	unsigned MCS:6;
	unsigned PL:16;
	unsigned DO:1;
	unsigned rvd:5;
} , PhyHdr );
class LayoutVLC;
class Parser : public gr::block
{
	public:
		typedef enum { PHR , PSDU } Type;
		typedef boost::shared_ptr<Parser> sptr;
		static sptr Create(Type, LayoutVLC * = NULL);
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		static void PHRParser(unsigned int);
		static void PSDUParser(unsigned char *);
		
	private:
		Parser(Type, LayoutVLC *);
		unsigned int ic;
		Type type;
		unsigned int PHRData;
		int psdu_len;
		unsigned char MHR[5];
		unsigned short payload_len;
		int payload_ver;
		QString payload;
		LayoutVLC *ly;
		unsigned int per;
		unsigned char prevSeq;
		unsigned int total;
		bool voip;
		bool prevreset;
		int cpd;
};

#endif //_INC_PARSER_H_
