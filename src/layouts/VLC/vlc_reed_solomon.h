#ifndef _VLC_REED_SOLOMON_H_
#define _VLC_REED_SOLOMON_H_


/*! \brief vlc_reed_solomon is a support class to perform the reed solomon encoding/decoding.

This class performs the reed solomon encoding/decoding process as defined in the IEEE 802.15.7.
In the blocks bbRSEnc, bbRSDec there is one instance of this class. These blocks call to the encode/decode method to perform the
proper operation. This implementation is based on the implementation of the reed solomon encoder/decoder found in the Phil Karn's website dedicated to 
Reed Solomon codes (http://www.ka9q.net/code/fec/)
*/

class vlc_reed_solomon {
	
	public:	
		/**
       * Constructor of the class vlc_reed_solomon. This method needs 5 parameters: 
       * @param _symsize: symbol size, bits
       * @param _gfpoly: Field generator polynomial coefficients
       * @param _fcr: first root of RS code generator polynomial, index form
       * @param _prim: primitive element to generate polynomial roots
       * @param _nroots: RS code generator polynomial degree (number of roots). The value is N-K
       */
        vlc_reed_solomon (unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
		~vlc_reed_solomon();
		/*!
		  * Encode RS packet
		*/
		void encode (unsigned char *rs_enc_out, unsigned char *in);
		unsigned int N;
		unsigned int K;
		/*! 
         * Decode RS encoded packet.
         * returns a count of corrected symbols, or -1 if the block was uncorrectible.
        */
		int decode (unsigned char *rs_dec_out, unsigned char *in); 
	
	private:
		void *d_rs;
		unsigned int rs_init_symsize;
		unsigned int rs_init_gfpoly;
		unsigned int rs_init_fcr;
		unsigned int rs_init_prim;
		unsigned int rs_init_nroots;
		
};

#endif /*_VLC_REED_SOLOMON_H_*/