#ifndef _INC_BBVLC_FRAME_EXTRACTOR_H
#define _INC_BBVLC_FRAME_EXTRACTOR_H

#include <gr_block.h>

/*! \brief bbVLC_Frame_Extractor block extracts the PHR or the PSDU blocks of data from the incoming frame

*  This blocks assumes that the synchronization has been performed through the TDP sequence. This block is thought to work with the following input:
   TDP + PHR + N*PSDU + IFS. The idea is to extract the PHR or the PSDU units (for the moment the n elements of the PSDU has the same content). 
*/

class bbVLC_Frame_Extractor : public gr_block
{
	public:
		typedef boost::shared_ptr<bbVLC_Frame_Extractor> sptr;
	   /**
       * The creation of the bbVLC_Frame_Extractor requires 5 parameters: 
       * @param _flag: this variable identifies which part to select: 0:PHR, 1:PSDU.
       * @param _tx_mode: this variable identifies which IFS has to be taken into account depending on whether we transmit in single, 
       * @param _d_mode: to indicate whether OOK or VPPM is being used 
       * @param _PHR_mod_length: length in bits of the modulated PHR block.
       * @param _PSDU_mod_length: length in bits of a single modulated PSDU block.
       * @param _PSDU_units: number of modulated PSDU units in the current frame structure. 
       */
		static sptr Create(int, int, int, int, int, int);
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		~bbVLC_Frame_Extractor();
		
	
	private:
		bbVLC_Frame_Extractor(int, int, int, int, int, int);
		int flag;
		int begin; //point to start to extract the samples corresponding to the desired part of the frame
		int end; // point to end to extract the samples corresponding to the desired part of the frame
		int length_sequence;  //length of the frame 
		int IFS; //constraint length
		int sample_counter;
		int d_mode;
};

#endif //_INC_BBVLC_FRAME_EXTRACTOR_H