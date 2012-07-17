#ifndef _INC_BBVLCINTERLEAVER_H
#define _INC_BBVLCINTERLEAVER_H

#include <gr_block.h>


/*! \brief bbVLCInterleaver block performs the interleaving operations defined in section 10.3 of IEEE 802.15.7.

This interleaving block is used between inner convolutional code and the outer Reed Solomon code. 
The interleaver is of a fixed height but has a flexible depth D, dependent on the frame size. 

*/

class bbVLCInterleaver : public gr_block
{
	public:
		typedef boost::shared_ptr<bbVLCInterleaver> sptr;
	   /**
       * The creation of the bbVLCInterleaver block requires 5 parameters: 
       * @param _GF: depth in bits of the Galois Field used in the Reed Solomon encoding process. (PHY I=4 bits, PHYII=8 bits)
       * @param _N: the output number of elements of an encoded word.
       * @param _K: the minimum number of elements to obtain a N-element encoded word.
       * @param _raw_length: length in bits at the input of the rs_encoder
       * @param _rs_length: length at the output of the rs_encoder (in GF space elements)
       */
		static sptr Create(unsigned int,unsigned int, unsigned int, unsigned int, unsigned int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bbVLCInterleaver();
		int out_int; /**< [out] indicates the length in bits of the output of the bbVLCInterleaver block*/
	
	private:
		bbVLCInterleaver(unsigned int,unsigned int, unsigned int, unsigned int, unsigned int);
		unsigned int GF; //number of bits of each word
		unsigned int N; //the length of the output reed-solomon codeword
		unsigned int K; //the length of the input reed-solomon codeword
		unsigned int raw_length; //length at the input of the rs_encoder
		unsigned int rs_length; //length at the output of the rs_encoder
		
		int *interleaving_vector;
		int *puncturing_vector;
		int *bin_number;
		void dec2bi(int , int, int *);
		bool puncture;
};

#endif //_INC_BBVLCINTERLEAVER_H