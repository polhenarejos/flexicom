// $Id$
#ifndef _VLC_CRC_H_
#define _VLC_CRC_H_

/*! \brief vlc_crc is a support class to compute the 16 bits crc over a set of bits.

This class performs the crc generation/checking process partially as defined in the IEEE 802.15.7.
The IEEE 802.15.7 defines the CRC computation in the Annex C. It corresponds to the CRC-16-CCITT procedure.
The generator polynomial is: x16 + x12 + x5 + 1.
The here in implementation differs slightly regarding the implementation defined in the IEEE 802.15.7 standard: shift registers are initialized to zero instead to one stated in the standard.
*/


class vlc_crc {
	
	public:	
		/**
       * Constructor of the vlc_crc class.  
       */
		//vlc_crc (int );
		vlc_crc();
		~vlc_crc();
		/**
         * Computes and appends the 16-bit CRC. 
        */
		void generate_crc(int *, int *, int);
		/**
		 * Checks the integrity of the incoming block of data
		*/
		bool check_crc (int *, int *, int);
		
	private:
		int poly[4];
		//int length_data;
		int crc_length;		
		
};

#endif /*_VLC_CRC_H_*/
