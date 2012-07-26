// $Id$
#ifndef _INC_BBVLCDEINTERLEAVER_H
#define _INC_BBVLCDEINTERLEAVER_H

#include <gr_block.h>


/*! \brief bbVLCDeInterleaver block performs the deinterleaving operation of IEEE 802.15.7.

The deinterleaving block is used between inner convolutional code and the outer Reed Solomon code. 
The deinterleaver is of a fixed height but has a flexible depth D, dependent on the frame size. 

*/

class bbVLCDeInterleaver : public gr_block
{
	public:
		typedef boost::shared_ptr<bbVLCDeInterleaver> sptr;
	   /**
       * The creation of the bbVLCDeInterleaver block requires 5 parameters: 
       * @param _GF: depth in bits of the Galois Field used in the Reed Solomon encoding process. (PHY I=4 bits, PHYII=8 bits)
       * @param _N: the output number of elements of an encoded word.
       * @param _K: the minimum number of elements to obtain a N-element encoded word.
       * @param _raw_length: length in bits at the input of the rs_encoder
       * @param _pre_length: length at the output of the rs_encoder (in GF space elements)
       */
       	static sptr Create(unsigned int,unsigned int, unsigned int, unsigned int, unsigned int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bbVLCDeInterleaver();
		int out_deint; /**< [out] indicates the length in GF words at the output of the bbVLCDeInterleaver block*/
	
	private:
		bbVLCDeInterleaver(unsigned int,unsigned int, unsigned int, unsigned int, unsigned int);
		unsigned int GF; //number of bits of each word
		unsigned int N; //the length of the output reed-solomon codeword
		unsigned int K; //the length of the input reed-solomon codeword
		unsigned int raw_length; //length at the input of the rs_encoder
		unsigned int pre_length; //length at the output of the previous block: cc_decoder or the output of the RLL decoder
		
		int *interleaving_vector;
		unsigned int *puncturing_vector;
		unsigned int len_punct_vector;
};

#endif //_INC_BBVLCDEINTERLEAVER_H
