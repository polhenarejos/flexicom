#ifndef _INC_BBVLC_FRAME_EXTRACTOR_H
#define _INC_BBVLC_FRAME_EXTRACTOR_H

#include <gr_block.h>

/*! \brief bbVLC_Frame_Extractor block extracts the PHR or the PSDU blocks of data from the incoming frame

*  This blocks assumes that the synchronization has been performed through the TDP sequence. This block is thought to work with the following input:
   TDP + PHR + N*PSDU + IFS. The idea is to extract the PHR or one unit of the PSDU (for the moment the n elements of the PSDU has the same content). 
*/

class bbVLC_Frame_Extractor : public gr_block
{
	public:
		typedef boost::shared_ptr<bbVLC_Frame_Extractor> sptr;
	   /**
       * The creation of the bbVLC_Frame_Extractor requires 5 parameters: 
       * @param _element_to_extract: this variable identifies which part to select: 0:PHR, 1:PSDU.
       * @param _tx_mode: this variable identifies which IFS has to be taken into account depending on whether we transmit in single, 
       * @param _poly: the generator polynomial. In VLC this variable should be set to g0 = 133; g1 = 171; g2 = 165 (octal values)
       * @param _length: length of the input block of data to perform the convolutional decoding.
       * @param _data_rate: an index indicating the data rate selected for the modulation and the PHY operating mode selected. See Table 73 and Table 74 of IEEE 802.15.7. This value defines the puncturing scheme to be applied.
       */
		static sptr Create(int, int, int, int, int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		~bbVLC_Frame_Extractor();
		int out_cc_dec; /**< [out] indicates the length in bits of the output of the bbVLC_Frame_Extractor block*/
	
	private:
		bbVLC_Frame_Extractor(int, int, int, int, int);
		int length_sequence; 
		int IFS; //constraint length
		int sample_counter;
};

#endif //_INC_BBVLC_FRAME_EXTRACTOR_H