#ifndef _VLC_CONVOLUTIONAL_CODING_H_
#define _VLC_CONVOLUTIONAL_CODING_H_

class vlc_convolutional_coding {
	
	public:	
		vlc_convolutional_coding (int, int, int *, int, int);
		~vlc_convolutional_coding();
		//void encode( int *, unsigned char *, int *, int *, int, int, int , int); 
		void encode( int *, int *, int *, int *, int, int, int , int); 
		//int encode_punct(int *, unsigned char *, int *, int *, int, int, int,int, int, int *);
		int encode_punct(int *, int *, int *, int *, int, int, int,int, int, int *);
		void decode(int *,int *,int ,int ,int ,int ,int *);
		void decode_punct(int *, int *, int, int , int , int, int *, int, int, int , int * );
		
		unsigned int N; //elements of the polynomial
		unsigned int K; //constraint length
		int *xor_table;
		int *punct_matrix;
		int *poly;
		int encoder_state;
		/*! 
         * Decode CC encoded packet.
        */
		int decode (unsigned char *rs_dec_out, unsigned char *in); 
	
	private:
		
		int *output_reverse_int;
		
		int no_states;
		int data_rate;
		int ones; //count the number of ones in the puncturing_matrix
		int length;
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