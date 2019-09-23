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

#include "InterPunct.h"
#include "Interleaver.h"
#include "Puncture.h"
#include "De2Bi.h"
#include "Bi2De.h"
#include <gnuradio/io_signature.h>

InterPunct::InterPunct(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length, Mode _mode) :
	gr::hier_block2("InterPunct", gr::io_signature::make(1, 1, sizeof(int)), gr::io_signature::make(1, 1, sizeof(int)))
{
	Interleaver::sptr ilv = Interleaver::Create(_GF, _N, _K, _raw_length, _rs_length, (_mode == INTERPUNCT ? Interleaver::INTERLEAVING : Interleaver::DEINTERLEAVING));
	if (_mode == INTERPUNCT)
	{
		Puncture::sptr pct = Puncture::Create(_GF, _N, _K, _raw_length, _rs_length);
		De2Bi::sptr de2bi = De2Bi::Create(_GF);
		connect(self(), 0, ilv, 0);
		connect(ilv, 0, pct, 0);
		connect(pct, 0, de2bi, 0);
		connect(de2bi, 0, self(), 0);
		out = pct->out*_GF;
	}
	else if (_mode == DEINTERPUNCT)
	{
		Bi2De::sptr bi2de = Bi2De::Create(_GF);
		connect(self(), 0, bi2de, 0);
		connect(bi2de, 0, ilv, 0);
		connect(ilv, 0, self(), 0);
		out = _rs_length;
	}
}
InterPunct::sptr InterPunct::Create(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length, Mode _mode)
{
	return gnuradio::get_initial_sptr(new InterPunct(_GF, _N, _K, _raw_length, _rs_length, _mode));
}
