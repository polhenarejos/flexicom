#ifndef _INC_TXVLC_H_
#define _INC_TXVLC_H_

#include <gr_hier_block2.h>
//#include <gr_msg_queue.h>
//#include <QThread>

/*class TxVLCThread : public QThread
{
	public:
		TxVLCThread(gr_msg_queue_sptr);
	protected:
		void run();
	private:
		gr_msg_queue_sptr queue;
};*/


struct rs_code
{
	int pre_rs_in;
	int pre_rs_out;
	int rs_in;
	int rs_out;
};

struct cc_code
{
	int pre_cc_in;
	int pre_cc_out;
	int cc_in;
	int cc_out;
};

//For simplicity, a struct which contains all the variables is created

struct VLCvar
{
	int phy_type;
	int mod_type;
	int clock_rate;
	int tx_mode;
	int psdu_units;
	int operating_mode;
	rs_code _rs_code;
	cc_code _cc_code;
	int GF;
	int frame_size;
	int PHR_raw_length; //raw length prior to modulation
	int PSDU_raw_length; //raw length prior to modulation
	int *MCSID; //field for the PHR preamble
	int flp_length;
};

class LayoutVLC;

/*! \brief The class where the layout of the VLC communication system is defined

* In this class, the TXVLC or the RXVLC is connected to the usrp class object to stablish the stream of samples
* Another functionality of this class is to manage the tab which configures the VLC system.

*/

class TxVLC : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<TxVLC> sptr;
	   /**
       * The creation of the TxVLC block requires 1 parameter: 
       * @param _ly: VLC layout, where the transmitter is connected to the usrp object. 
       */
		static sptr Create(LayoutVLC *);
		~TxVLC();
		void stop();
		VLCvar vlc_var; /**< [in] struct which contains the configuration of the VLC system set at the GUI interface.*/
		
		
	private:
		int *poly;
		TxVLC(LayoutVLC *);
		LayoutVLC *ly;
		void init_var();
};

#endif //_INC_TXVLC_H_
