// $Id$

#ifndef _INC_INTERLEAVER_H_
#define _INC_INTERLEAVER_H_

#include <gnuradio/sync_block.h>
#include <vector>

class Interleaver : public gr::sync_block
{
	public:
		typedef enum { INTERLEAVING, DEINTERLEAVING } Mode;
		typedef boost::shared_ptr<Interleaver> sptr;
		static sptr Create(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, Mode);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		static void Decode(const int *, int *, int, int, std::vector<unsigned int> &, Mode);
		
	private:
		Interleaver(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, Mode);
		unsigned int GF; //number of bits of each word
		unsigned int N; //the length of the output reed-solomon codeword
		unsigned int K; //the length of the input reed-solomon codeword
		unsigned int raw_length;
		unsigned int rs_length;
		std::vector<unsigned int> ivector;
		Mode mode;
};

#endif //_INC_INTERLEAVER_H_
