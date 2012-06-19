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

class LayoutVLC;
class TxVLC : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<TxVLC> sptr;
		static sptr Create(LayoutVLC *);
		~TxVLC();
		void stop();
		int phy_type;
		int mod_type;
		int clock_rate;
		
	private:
		TxVLC(LayoutVLC *);
		//for simplicity I read the variables from the GUI
		int tx_mode;
		int psdu_units;
		rs_code rs_code;
		cc_code cc_code;
		int GF;
		int frame_size;
		int PHR_raw_length; //raw length prior to modulation
		int PSDU_raw_length; //raw length prior to modulation
		LayoutVLC *ly;
		void init_var();
};

#endif //_INC_TXVLC_H_
