#include "vlc_reed_solomon.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

extern "C"{
	#include "rs.h"
}


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
   memcpy (rs_enc_out, in, K);
  // now compute parity bytes and add them to tail end of output packet
  encode_rs_char (d_rs, in, &rs_enc_out[K]);
}

int vlc_reed_solomon::decode(unsigned char *rs_dec_out, unsigned char *in)
{
  unsigned char *tmp;
  int ncorrections;

  tmp =new unsigned char[N];
  //assert ((sizeof(in)) == N); ->if it is a pointer this is going to fail
  memcpy (tmp, in, N);
  // correct message...
  ncorrections = decode_rs_char (d_rs, tmp, 0, 0);
  // copy corrected message to output, skipping prefix zero padding
  memcpy (rs_dec_out, tmp, K);
  return ncorrections;
}