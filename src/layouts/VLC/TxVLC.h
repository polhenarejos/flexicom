// $Id$
#ifndef _INC_TXVLC_H_
#define _INC_TXVLC_H_

#include <gr_hier_block2.h>
#include <boost/thread.hpp>
#include "DataSource.h"

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
	unsigned int pre_rs_in;
	unsigned int pre_rs_out;
	unsigned int rs_in;
	unsigned int rs_out;
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
	unsigned int phy_type;
	int mod_type;
	int clock_rate;
	int tx_mode;
	unsigned int psdu_units;
	int operating_mode;
	rs_code _rs_code;
	cc_code _cc_code;
	unsigned int GF;
	unsigned int frame_size;
	unsigned int PHR_raw_length; //raw length prior to modulation
	unsigned int PSDU_raw_length; //raw length prior to modulation
	int MCSID[6]; //field for the PHR preamble
	int flp_length;
	uint64_t count;
};

class LayoutVLC;

/*! \brief The class where the layout of the VLC communication system (transmitter part) is defined

* In this class, the TXVLC or the RXVLC is connected to the usrp class object to stablish the stream of samples
* Another functionality of this class is to read the tab which configures the VLC system.

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
		void UIChanged();
		uint64_t GetConfigVer();
		void UpdateConfigVer();
		int SendPacket(unsigned char *, unsigned int);
		
	private:
		int poly[3];
		TxVLC(LayoutVLC *);
		LayoutVLC *ly;
		void init_var();
		boost::mutex mutex;
		DataSource::sptr data_source;
};

#endif //_INC_TXVLC_H_
