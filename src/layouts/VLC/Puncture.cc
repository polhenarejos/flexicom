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

#include "Puncture.h"
#include <gnuradio/io_signature.h>

Puncture::Puncture(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length) :
	gr::block("Interleaver", gr::io_signature::make(1, 1, sizeof(int)), gr::io_signature::make(1, 1, sizeof(int))),
	rs_length(_rs_length), ic(0)
{
	unsigned int S_frame = ceil((double)_raw_length/_GF);
	unsigned int S = _N * ceil((double)S_frame/_K) -  (_K- (S_frame%_K));
	unsigned int D = ceil(((double)S/_N));
	unsigned int p = _N - (S%_N);
	out = rs_length;
	if (p < _K)
	{
		for (unsigned int i = 0; i < p; i++)
			pvector.push_back((_N-p+1)*D + (i*D)-1);
		out -= p;
	}
	set_output_multiple(out);
}
Puncture::sptr Puncture::Create(unsigned int _GF, unsigned int _N, unsigned int _K, unsigned int _raw_length, unsigned int _rs_length)
{
	return sptr(new Puncture(_GF, _N, _K, _raw_length, _rs_length));
}
//returns p, hence the output is OutRS()-OutPunct()
int Puncture::OutPunct(int _raw_length, int _GF, int _N, int _K)
{
	unsigned int S_frame = ceil((double)_raw_length/_GF);
	unsigned int S = _N * ceil((double)S_frame/_K) -  (_K- (S_frame%_K));
	unsigned int D = ceil(((double)S/_N));
	unsigned int p = _N - (S%_N);
	return (p < _K ? p : 0);
}
void Puncture::forecast(int no, gr_vector_int &ni)
{
	for (int n = 0; n < ni.size(); n++)
		ni[n] = (no/out)*(out+pvector.size());
}
int Puncture::Encode(const int *iptr, int *optr, int no, int rs_length, std::vector<unsigned int> &pvector, unsigned int &ic)
{
	int csmd = 0;
	for (int n = 0; n < no; n++)
	{
		while (std::find(pvector.begin(), pvector.end(), ic) != pvector.end())
		{
			ic = (ic+1)%rs_length;
			csmd++;
		}
		*optr++ = *(iptr+csmd++);
		ic = (ic+1)%rs_length;
	}
	return csmd;
}
#include <QMutex>
extern QMutex mtx;
int Puncture::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &_i, gr_vector_void_star &_o) 
{
	const int *iptr = (const int *)_i[0];
	int *optr = (int *)_o[0];
	int csmd = Encode(iptr, optr, no, rs_length, pvector, ic);
	consume_each(csmd);
	return no;
}
