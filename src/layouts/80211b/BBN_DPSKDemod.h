#ifndef _INC_BBN_DPSKDEMOD_H_
#define _INC_BBN_DPSKDEMOD_H_

#include <gr_block.h>

#define RSSI_AVE_COUNT 128
#define RSSI_MAX 0

class BBN_DPSKDemod : public gr_block 
{
	public:
		typedef boost::shared_ptr<BBN_DPSKDemod> sptr;
		static sptr Create();
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		void forecast(int, gr_vector_int &);
		
	private:
		BBN_DPSKDemod();
		void UpdateRSSI();
		void InitLogTable();
		gr_complex d_prev;
		float d_e_squared;
		int d_sample_count;
		short d_rssi;
		char d_log_table[64];
};

#endif //_INC_BBN_DPSKDEMOD_H_
