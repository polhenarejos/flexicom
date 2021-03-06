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

#include "vlc_reed_solomon.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
extern "C" {
#include <gnuradio/fec/rs.h>
};

vlc_reed_solomon::vlc_reed_solomon (unsigned int _symsize, unsigned int _gfpoly, unsigned int _fcr, unsigned int _prim, unsigned int _nroots) :
				 rs_init_symsize(_symsize), rs_init_gfpoly(_gfpoly), rs_init_fcr (_fcr), rs_init_prim(_prim), rs_init_nroots(_nroots)
{
  //printf("rs_init_symsize:%d, rs_init_gfpoly:%d, rs_init_fcr:%d\n", rs_init_symsize, rs_init_gfpoly, rs_init_fcr);
  d_rs = (struct rs *)init_rs_char(rs_init_symsize, rs_init_gfpoly, rs_init_fcr, rs_init_prim, rs_init_nroots);
  N = (1 << rs_init_symsize) - 1;
  K = N - rs_init_nroots;
  assert (d_rs != 0);
}

vlc_reed_solomon::~vlc_reed_solomon ()
{
  if (d_rs)
  {
    free_rs_char (d_rs);
    d_rs = 0;
  }
}

void vlc_reed_solomon::encode(unsigned char *rs_enc_out, unsigned char *in)
{
   //assert ((sizeof(in)) == K); ->if it is a pointer this is going to fail

  // copy message portion to output packet
   memcpy (rs_enc_out, in, sizeof(unsigned char)*K);
  // now compute parity bytes and add them to tail end of output packet
  encode_rs_char (d_rs, in, &rs_enc_out[K]);
}

int vlc_reed_solomon::decode(unsigned char *rs_dec_out, unsigned char *in)
{
  unsigned char *tmp = new unsigned char[N];
  int ncorrections;

  //assert ((sizeof(in)) == N); ->if it is a pointer this is going to fail
  memcpy (tmp, in, sizeof(unsigned char)*N);
  // correct message...
  ncorrections = decode_rs_char (d_rs, tmp, 0, 0);
  // copy corrected message to output, skipping prefix zero padding
  memcpy (rs_dec_out, tmp, sizeof(unsigned char)*K);
  delete [] tmp;
  return ncorrections;
}
