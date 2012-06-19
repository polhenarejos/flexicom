#ifndef _VLC_REED_SOLOMON_H_
#define _VLC_REED_SOLOMON_H_

class vlc_reed_solomon {
	
	public:	
		vlc_reed_solomon (unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
		~vlc_reed_solomon();
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