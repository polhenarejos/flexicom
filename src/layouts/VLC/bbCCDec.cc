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

#include "bbCCDec.h"
#include <gnuradio/io_signature.h>
#include "vlc_convolutional_coding.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


bbCCDec::bbCCDec( int _N, int _K, int *_poly, int _length, int _data_rate):
	gr::block("bbCCDec", gr::io_signature::make (1,1, sizeof(int)), gr::io_signature::make (1,1, sizeof(int))),
	N(_N), K(_K), poly(_poly),length(_length), data_rate(_data_rate)
{
	vlc_cc = new vlc_convolutional_coding(N, K, poly, length, data_rate);
	//printf("Valor de N:%d, de K:%d, de poly[1]:%d, length:%d, data_rate:%d\n", N,K,poly[1],length, data_rate);
	out_cc_dec = OutCC(length, K, data_rate);
	//printf("El valor de out_cc_dec:%d %d %d %d\n", out_cc_dec,data_rate,K,length);
	set_output_multiple(out_cc_dec);	
}

bbCCDec::~bbCCDec()
{
	if(vlc_cc)
	{
		delete vlc_cc;
		vlc_cc = 0;
	}
}
int bbCCDec::OutCC(int length, int K, int data_rate)
{
	if (data_rate == 0)
		return length/2-(K-1);
	else if (data_rate == 1)
		return length/3-(K-1);
	else if (data_rate == 2)
		return length*2/3-(K-1);
	return 0;
}
bbCCDec::sptr bbCCDec::Create(int _N, int _K, int *_poly, int _length, int _data_rate)
{
	return sptr(new bbCCDec(_N, _K, _poly, _length, _data_rate));
}

void bbCCDec::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	int ninputs = ninput_items_required.size();
	for (int i=0; i < ninputs; i++)
		ninput_items_required[i]= (noutput_items/out_cc_dec)*length;
		
	
}

int bbCCDec::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	const int *iptr= (const int *)input_items[0];
	int *optr= (int *)output_items[0];
	
	int blocks_to_process, i;
	int size;
	int *tmp;
	int *tmp2;
	//tmp= new int[length];
	tmp2=new int[out_cc_dec];
	blocks_to_process = (noutput_items/out_cc_dec);
	while (blocks_to_process>0)
	{
		memset(tmp2,0,sizeof(int)*out_cc_dec);
		tmp = new int[length];
		size= length;
		memcpy(tmp, iptr, sizeof(int)*length);
		iptr = iptr + length;
		vlc_cc->decode_punct(tmp,tmp2, out_cc_dec, K, N, vlc_cc->no_states, vlc_cc->output_reverse_int, size, vlc_cc->ones,2,vlc_cc->punct_matrix);
		memcpy(optr, tmp2, sizeof(int)*out_cc_dec);
		optr = optr + out_cc_dec;
		blocks_to_process--;
		delete [] tmp; 
	}
	
	delete [] tmp2;
	
	consume_each((noutput_items/out_cc_dec)*length);
	return (noutput_items/out_cc_dec)*out_cc_dec;
}
