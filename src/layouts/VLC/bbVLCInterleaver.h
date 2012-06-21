#ifndef _INC_BBVLCINTERLEAVER_H
#define _INC_BBVLCINTERLEAVER_H

#include <gr_block.h>



class bbVLCInterleaver : public gr_block
{
	public:
		typedef boost::shared_ptr<bbVLCInterleaver> sptr;
		static sptr Create(unsigned int,unsigned int, unsigned int, int, int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bbVLCInterleaver();
	
	private:
		bbVLCInterleaver(unsigned int,unsigned int, unsigned int, int, int);
		unsigned int N;
		unsigned int K;
		unsigned int GF;
		int raw_length;
		int rs_length;
		int out_int; //remaining elements to be punctured
		int *interleaving_vector;
		int *puncturing_vector;
		int *bin_number;
		void dec2bi(unsigned char *, int, int *);
		bool puncture;
};

#endif //_INC_BBVLCINTERLEAVER_H