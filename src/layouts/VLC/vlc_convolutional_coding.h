#ifndef _VLC_CONVOLUTIONAL_CODING_H_
#define _VLC_CONVOLUTIONAL_CODING_H_

/*! \brief vlc_convolutional_coding is a support class to perform the convolutional encoding/decoding.

This class performs the convolutional encoding/decoding process as defined in the IEEE 802.15.7.
In the blocks bbCCEnc, bbCCDec there is one instance of this class. These blocks call to the encode/decode method to perform the
proper operation. This implementation is based on the implementation of the convolutional encoder/decoder found at the IT++ libraries
*/

class vlc_convolutional_coding {
	
	public:	
		/**
       * Constructor of the class vlc_convolutional_coding. This method needs 5 parameters: 
       N(_N),K(_K),poly(_poly), length(_length), data_rate(_data_rate)
       * @param _N: the rate of the convolutional code. In VLC this variable should be set to 3.
       * @param _K: the constraint length of the code. In VLC this variable should be set to 7.
       * @param _poly: the generator polynomial. In VLC this variable should be set to g0 = 133; g1 = 171; g2 = 165 (octal values)
       * @param _length: length of the input block of data to perform the convolutional encoding.
       * @param _data_rate: an index indicating the data rate selected for the modulation and the PHY operating mode selected. See Table 73 and Table 74 of IEEE 802.15.7. This value defines the puncturing scheme to be applied.
       */
		vlc_convolutional_coding (int, int, int *, int, int);
		~vlc_convolutional_coding();
		//void encode( int *, unsigned char *, int *, int *, int, int, int , int); 
		void encode( int *, int *, int *, int *, int, int, int , int); 
		//int encode_punct(int *, unsigned char *, int *, int *, int, int, int,int, int, int *);
		/*!
		 * Performs puntured CC encoding of a block of data.
		*/
		int encode_punct(int *, int *, int *, int *, int, int, int,int, int, int *);
		void decode(int *,int *,int ,int ,int ,int ,int *);
		/*! 
        * Decode punctured CC encoded block of data.
        */
		void decode_punct(int *, int *, int, int , int , int, int *, int, int, int , int * );
		
		unsigned int N; //elements of the polynomial
		unsigned int K; //constraint length
		int *xor_table;
		int *punct_matrix; /**< Puncturing matrix. Supported rates: 1/4, 1/3, 2/3 */
		int *poly; /**< Array containing the generator polynomial. The length of this array is equal to the N value */
		int encoder_state;
		
		
	
	private:
		
		int *output_reverse_int;
		
		int no_states;
		int length;
		int data_rate;
		int ones; //count the number of ones in the puncturing_matrix
		//encoding auxiliary functions
		void set_generator_polynomials(int *, int *, int , int ,int, int *);
		int weight_int(int, int);
		void output_reverse(int, int, int *, int *, int *);
		//decoding auxiliary functions
		void calc_metric(int *rx_codeword, int *delta_metrics,int n);
		int previous_state_1(int state, int input, int m);
		void previous_state_2(int state, int *S0, int no_states);
		int get_input(int state,int m);
		int min_index(int *sum_metric, int no_states);
		
};
#endif /*_VLC_CONVOLUTIONAL_CODING_H_*/