#ifndef _INC_BBN_PLCP_H_
#define _INC_BBN_PLCP_H_

#include <gr_block.h>
#include <gr_sync_block.h>
#include <gr_msg_queue.h>
#include <QtGlobal>
#include "compat.h"

#define SFD (0x05cf)
#define RSFD (0xf3a0)
#define MAX_PDU_LENGTH (2500)

PACK(
typedef struct oob_hdr_struct 
{
	long long timestamp; /* Relative time in microseconds */
	ushort length;
	signed char rssi; /* dB Scale */
	char rate; /* Receive rate in units of 100 kBps */
} oob_hdr_t);

#define REVERSE_BITS(c) ( bit_reverse_table[(c) & 0xff])

class BBN_PLCP : public gr_block 
{
	public:
		typedef boost::shared_ptr<BBN_PLCP> sptr;
		static sptr Create(gr_msg_queue_sptr, bool = true);
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		void forecast(int, gr_vector_int &);
		static uchar ReverseBits(uchar);
		~BBN_PLCP();
		enum plcp_state_enum 
		{
			PLCP_STATE_SEARCH_PREAMBLE,
			PLCP_STATE_SEARCH_SFD1,
			PLCP_STATE_SEARCH_SFD2,
			PLCP_STATE_SEARCH_SFD3,
			PLCP_STATE_SEARCH_RSFD1,
			PLCP_STATE_SEARCH_RSFD2,
			PLCP_STATE_SEARCH_RSFD3,
			PLCP_STATE_HDR,
			PLCP_STATE_SHDR,
			PLCP_STATE_PDU
		};
		enum plcp_current_rate_enum 
		{
			PLCP_RATE_1MBPS,
			PLCP_RATE_2MBPS
		};
		
	private:
		BBN_PLCP(gr_msg_queue_sptr, bool = true);
		typedef struct scramble_data_struct 
		{
			uchar data;
			uchar seed;
		} scramble_data_t;
		long long d_symbol_count;
		long long d_packet_rx_time;
		long long d_packet_rate;
		gr_msg_queue_sptr d_target_queue;
		plcp_state_enum d_state;
		bool crc_table_initialized;
		uint d_data;
		int d_byte_offset;
		int d_shift;
		int d_byte_count;
		int d_pdu_len;
		plcp_current_rate_enum d_rate;
		ushort d_data1_in;
		uint d_data2_in;
		uint d_check_crc;
		uchar d_hdr[6];
		uchar bit_reverse_table[256];
		uchar d_pkt_data[MAX_PDU_LENGTH];
		uchar d_scrambler_seed;
		int ProcessByte(uchar *, uchar *);
		int d_shift_register;
		scramble_data_t *d_table;
		int DescrambleBits(uchar *byte, uchar *seed, int num_bits);
		void CRC16Init(void);
		ushort CRC16(const uchar *buf, int len);
		uint CRC32(const uchar *buf, int len);
		uint UpdateCRC(uint crc, const uchar *data, int len, uint);
		ushort crc16_table[256];
		uint crc32_table[256];
};

#endif //_INC_BBN_PLCP_H_
