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

#include "Interleaver.h"
#include <gnuradio/io_signature.h>

Interleaver::Interleaver(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length, Mode _mode) :
	gr::sync_block("Interleaver", gr::io_signature::make(1, 1, sizeof(int)), gr::io_signature::make(1, 1, sizeof(int))),
	GF(_GF), N(_N), K(_K), raw_length(_raw_length), rs_length(_rs_length), mode(_mode)
{
	unsigned int S_frame = ceil((double)raw_length/GF);
	unsigned int S = N * ceil((double)S_frame/K) -  (K- (S_frame%K));
	unsigned int D = ceil(((double)S/N));
	unsigned int S_block = N * D;
	unsigned int p = N - (S%N);
	//ivector.resize(S_block);
	//printf("!! %d %d %d %d %d %d %d %d\n",raw_length,rs_length,S_block,S_frame,S,D,N,K);
	for (unsigned int i = 0; i < S_block; i++)
		ivector.push_back((i%D)*N + i/D);
	set_output_multiple(rs_length);
}
Interleaver::sptr Interleaver::Create(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length, Mode _mode)
{
	return sptr(new Interleaver(_GF, _N, _K, _raw_length, _rs_length, _mode));
}
void Interleaver::Decode(const int *iptr, int *optr, int no, int rs_length, std::vector<unsigned int> &ivector, Mode mode)
{
	for (int n = 0; n < no/rs_length; n++)
	{
		for (int i = 0; i < ivector.size(); i++)
		{
			if (mode == INTERLEAVING)
				*optr++ = *(iptr+ivector[i]);
			else if (mode == DEINTERLEAVING && ivector[i] < rs_length)
				*(optr+ivector[i]) = *iptr++;
		}
		if (mode == INTERLEAVING)
			iptr += rs_length;
		else
			optr += rs_length;
	}
}
int Interleaver::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o) 
{
	const int *iptr = (const int *)_i[0];
	int *optr = (int *)_o[0];
	Decode(iptr, optr, no, rs_length, ivector, mode);
	return no;
}
