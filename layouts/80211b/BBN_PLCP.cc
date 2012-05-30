#include "BBN_PLCP.h"
#include <gr_io_signature.h>

#define PLCP_DEBUG 0

static inline uchar ReverseBits(uchar);

BBN_PLCP::BBN_PLCP(gr_msg_queue_sptr target_queue, bool check_crc) :
	gr_block ("BBN_PLCP", gr_make_io_signature (1, 2, sizeof (ushort)), gr_make_io_signature(0,0,0)),
	d_symbol_count(0), d_packet_rx_time(0), d_packet_rate(0), d_target_queue(target_queue), d_state(PLCP_STATE_SEARCH_PREAMBLE), crc_table_initialized(false),
	d_data(0), d_byte_count(0), d_pdu_len(0), d_rate(PLCP_RATE_1MBPS), d_data1_in(0), d_data2_in(0), d_check_crc(check_crc), d_scrambler_seed(0x62) 
{
	set_relative_rate((float)1);
	for (int i=0; i<256; ++i)
		bit_reverse_table[i] = ReverseBits((uchar)i);
	d_table = new scramble_data_t[256 * 128];
	for (int i=0; i<(128 * 256); ++i) 
	{
		d_table[i].data = i & 0xff;
		d_table[i].seed = i >> 8;
		DescrambleBits(&d_table[i].data, &d_table[i].seed, 8);
	}
	d_shift_register = 0;
}
BBN_PLCP::~BBN_PLCP()
{
	if (d_table) 
		delete d_table;
}
BBN_PLCP::sptr BBN_PLCP::Create(gr_msg_queue_sptr target_queue)
{
	return sptr(new BBN_PLCP(target_queue));
}
uchar BBN_PLCP::ReverseBits(uchar c) 
{
	uchar result;
	result = c & 0x01;
	result <<= 1;
	c >>= 1;
	result |= c & 0x01;
	result <<= 1;
	c >>= 1;
	result |= c & 0x01;
	result <<= 1;
	c >>= 1;
	result |= c & 0x01;
	result <<= 1;
	c >>= 1;
	result |= c & 0x01;
	result <<= 1;
	c >>= 1;
	result |= c & 0x01;
	result <<= 1;
	c >>= 1;
	result |= c & 0x01;
	result <<= 1;
	c >>= 1;
	result |= c & 0x01;
	return result;
}
void BBN_PLCP::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	uint ninputs = ninput_items_required.size();
	for (uint i = 0; i < ninputs; i++)
		ninput_items_required[i] = noutput_items + 3;
}
int BBN_PLCP::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	int num_output = 0;
	uint sfd_search;
	ushort crc;
	struct plcp_hdr_t 
	{
		uchar signal;
		uchar service;
		ushort length;
		ushort crc;
	} plcp_hdrp;
	uchar descrambled_byte;
	uchar descrambled_byte2[2] = {0, 0};
	uchar saved_scrambler_seed;
	int symbol_delta;
	oob_hdr_t *oob;
	int i;
	ushort *iptr1 = (ushort *) input_items[0];
	ushort *iptr2 = (ushort *) input_items[1];
	num_output = 0;
	for (i=0; i<noutput_items; ++i) 
	{
		saved_scrambler_seed = d_scrambler_seed;
		d_data1_in = (d_data1_in << 8) | (*iptr1 & 0xff);
		d_data2_in = (d_data2_in << 16) | *iptr2;
		if (d_rate == PLCP_RATE_1MBPS) 
		{
			descrambled_byte = (d_data1_in >> d_shift) & 0xff;    
			ProcessByte(&descrambled_byte, &d_scrambler_seed);
		} 
		else 
		{
			descrambled_byte2[0] = (d_data2_in >> (d_shift + 8)) & 0xff;
			ProcessByte(&descrambled_byte2[0], &d_scrambler_seed);
			descrambled_byte2[1] = (d_data2_in >> d_shift) & 0xff;
			ProcessByte(&descrambled_byte2[1], &d_scrambler_seed);
		}
		switch(d_state) 
		{
			case PLCP_STATE_SEARCH_PREAMBLE:
				if (descrambled_byte == 0xff)
					d_state = PLCP_STATE_SEARCH_SFD1;
				else if (descrambled_byte == 0x00)
					d_state = PLCP_STATE_SEARCH_RSFD1;
				break;
			case PLCP_STATE_SEARCH_SFD1:
				if (descrambled_byte != 0xff) 
				{
					d_data = 0xff000000 | ((uint)descrambled_byte) << 16;
					d_state = PLCP_STATE_SEARCH_SFD2;
				}
				break;
			case PLCP_STATE_SEARCH_SFD2:
				d_data |= ((uint)descrambled_byte) << 8;
				d_state = PLCP_STATE_SEARCH_SFD3;
				break;
			case PLCP_STATE_SEARCH_SFD3:
				d_data |= ((uint)descrambled_byte);
				sfd_search = d_data;
				for (d_shift=0; d_shift<8; ++d_shift) 
				{
					if ((sfd_search & 0x00FFFF00) == (SFD << 8))
						break;
					sfd_search <<= 1;
				}
				if (d_shift == 8) 
				{
					d_state = PLCP_STATE_SEARCH_PREAMBLE;
					d_shift = 0;
					break;
				}
				descrambled_byte = d_data1_in & 0xff;
				DescrambleBits(&descrambled_byte, &saved_scrambler_seed, d_shift);
				d_shift = (8 - d_shift);
				d_scrambler_seed = saved_scrambler_seed;
				d_state = PLCP_STATE_HDR;
				d_byte_count = 0;
				break;
			case PLCP_STATE_SEARCH_RSFD1:
				if (descrambled_byte != 0xff) 
				{
					d_data = ((uint)descrambled_byte) << 16;
					d_state = PLCP_STATE_SEARCH_RSFD2;
				}
				break;
			case PLCP_STATE_SEARCH_RSFD2:
				d_data |= ((uint)descrambled_byte) << 8;
				d_state = PLCP_STATE_SEARCH_RSFD3;
				break;
			case PLCP_STATE_SEARCH_RSFD3:
				d_data |= ((uint)descrambled_byte);
				sfd_search = d_data;
				for (d_shift=0; d_shift<8; ++d_shift) 
				{
					if ((sfd_search & 0x00FFFF00) == (RSFD << 8))
						break;
					sfd_search <<= 1;
				}
				if (d_shift == 8) 
				{
					d_state = PLCP_STATE_SEARCH_PREAMBLE;
					d_shift = 0;
					break;
				}
				descrambled_byte = d_data1_in & 0xff;
				DescrambleBits(&descrambled_byte, &saved_scrambler_seed, d_shift);
				d_shift = (8 - d_shift) << 1;
				d_scrambler_seed = saved_scrambler_seed;
				d_rate = PLCP_RATE_2MBPS;
				d_state = PLCP_STATE_SHDR;
				d_byte_count = 0;
				break;
			case PLCP_STATE_HDR:
				d_hdr[d_byte_count++] = REVERSE_BITS(descrambled_byte);
				if (d_byte_count == 6) 
				{
					plcp_hdrp.signal = d_hdr[0];
					plcp_hdrp.service = d_hdr[1];
					plcp_hdrp.length = (((ushort)d_hdr[3]) << 8) | d_hdr[2];
					plcp_hdrp.crc = (((ushort)d_hdr[5]) << 8) | d_hdr[4];
					crc = CRC16(d_hdr, (size_t)4);
#if PLCP_DEBUG
					printf("Recieved header!\n");
					printf("  signal: 0x%02X\n", plcp_hdrp.signal);
					printf("  service: 0x%02X\n", plcp_hdrp.service);
					printf("  length: 0x%04X\n", plcp_hdrp.length);
					printf("  crc: 0x%04X\n", plcp_hdrp.crc);
					printf("Calculated crc: 0x%04X\n", crc);
#endif
					if (plcp_hdrp.crc != crc) 
					{
						d_state = PLCP_STATE_SEARCH_PREAMBLE;
						d_shift = 0;
#if PLCP_DEBUG
						printf("*** BAD CRC ***\n\n");
#endif
						break;
					}
					if (plcp_hdrp.signal == 0x0a) 
					{
						/* 1 Mbps */
						d_pdu_len = plcp_hdrp.length >> 3;
						d_state = PLCP_STATE_SEARCH_PREAMBLE;
					} 
					else if (plcp_hdrp.signal == 0x14) 
					{
						/* 2 Mbps */
#if PLCP_DEBUG
						printf("Running at 2Mbps\n");
#endif
						d_rate = PLCP_RATE_2MBPS;
						d_shift <<= 1;
						d_pdu_len = plcp_hdrp.length >> 2;
					} 
					else 
					{
						/* Rate is not 1 Mpbs or 2 Mbps */
						d_state = PLCP_STATE_SEARCH_PREAMBLE;
						d_shift = 0;
#if PLCP_DEBUG
						printf("Unsupported Rate = %2.1f Mbps\n", (float)(plcp_hdrp.signal) / 10.0);
#endif
						break;
					}
#if PLCP_DEBUG
					printf("Packet Length: %d bytes.\n", d_pdu_len);
#endif
					if (d_pdu_len > MAX_PDU_LENGTH) 
					{
#if PLCP_DEBUG
						printf("Ignoring packet because it is too long\n");
#endif
						d_state = PLCP_STATE_SEARCH_PREAMBLE;
						d_shift = 0;
						d_rate = PLCP_RATE_1MBPS;
						break;
					}
					d_state = PLCP_STATE_PDU;
					d_byte_count = 0;
					d_packet_rate = plcp_hdrp.signal;
					symbol_delta = (((ulong)iptr1 - (ulong)input_items[0]) << 3);
					d_packet_rx_time = d_symbol_count + (long long)symbol_delta;
				}
				break;
			case PLCP_STATE_SHDR:
				d_hdr[d_byte_count++] = REVERSE_BITS(descrambled_byte2[0]);
				d_hdr[d_byte_count++] = REVERSE_BITS(descrambled_byte2[1]);
				if (d_byte_count == 6) 
				{
					plcp_hdrp.signal = d_hdr[0];
					plcp_hdrp.service = d_hdr[1];
					plcp_hdrp.length = (( (ushort)d_hdr[3]) << 8) | d_hdr[2];
					plcp_hdrp.crc = (( (ushort)d_hdr[5]) << 8) | d_hdr[4];
					crc = CRC16(d_hdr, (size_t)4);
#if PLCP_DEBUG
					printf("Recieved (short) header!\n");
					printf("  signal: 0x%02X\n", plcp_hdrp.signal);
					printf("  service: 0x%02X\n", plcp_hdrp.service);
					printf("  length: 0x%04X\n", plcp_hdrp.length);
					printf("  crc: 0x%04X\n", plcp_hdrp.crc);
					printf("Calculated crc: 0x%04X\n", crc);
#endif
					if (plcp_hdrp.crc != crc) 
					{
						d_state = PLCP_STATE_SEARCH_PREAMBLE;
						d_rate = PLCP_RATE_1MBPS;
						d_shift = 0;
#if PLCP_DEBUG
						printf("*** BAD CRC ***\n\n");
#endif
						break;
					}
					if (plcp_hdrp.signal == 0x14) 
					{
						/* 2 Mbps */
						d_pdu_len = plcp_hdrp.length >> 2;
					} 
					else 
					{
						/* Rate is not 1 Mpbs or 2 Mbps */
						d_state = PLCP_STATE_SEARCH_PREAMBLE;
						d_rate = PLCP_RATE_1MBPS;
						d_shift = 0;
#if PLCP_DEBUG
						printf("Unsupported Short Pramble Rate = %2.1f Mbps\n", (float)(plcp_hdrp.signal) / 10.0);
#endif
						break;
					}
#if PLCP_DEBUG
					printf("Packet Length: %d bytes.\n", d_pdu_len);
#endif    
					if (d_pdu_len > MAX_PDU_LENGTH) 
					{
#if PLCP_DEBUG
						printf("Ignoring packet because it is too long\n");
#endif
						d_state = PLCP_STATE_SEARCH_PREAMBLE;
						d_rate = PLCP_RATE_1MBPS;
						d_shift = 0;
						break;
					}
					d_state = PLCP_STATE_PDU;
					d_byte_count = 0;
					d_packet_rate = plcp_hdrp.signal;
					symbol_delta = (((ulong)iptr1 - (ulong)input_items[0]) << 3);
					d_packet_rx_time = d_symbol_count + (long long)symbol_delta;
				}
				break;
			case PLCP_STATE_PDU:
				if (d_rate == PLCP_RATE_1MBPS) 
				{
					d_pkt_data[d_byte_count++] = REVERSE_BITS(descrambled_byte);
#if PLCP_DEBUG
					printf("%u ", d_pkt_data[d_byte_count]);
#endif
				} 
				else 
				{
					d_pkt_data[d_byte_count++] = REVERSE_BITS(descrambled_byte2[0]);
#if PLCP_DEBUG
					printf("%u ", d_pkt_data[d_byte_count]);
#endif
					d_pkt_data[d_byte_count++] = REVERSE_BITS(descrambled_byte2[1]);
#if PLCP_DEBUG
					printf("%u ", d_pkt_data[d_byte_count]);
#endif
				}
				if (d_byte_count >= d_pdu_len) 
				{
#if PLCP_DEBUG
					printf("\n");
#endif
					if (d_check_crc) 
					{
						if (CRC32(d_pkt_data, d_pdu_len) != 0x2144df1c) 
						{
							/* Payload crc check failed */
							d_state = PLCP_STATE_SEARCH_PREAMBLE;
							d_shift = 0;
							d_rate = PLCP_RATE_1MBPS;
							break;
						}
						d_pdu_len -= 4; /* Strip the crc from the payload */
					}
					gr_message_sptr msg = gr_make_message(0, 0, 0, d_pdu_len + sizeof(oob_hdr_t));
					memcpy(msg->msg() + sizeof(oob_hdr_t), d_pkt_data, d_pdu_len);
					oob = (oob_hdr_t *)msg->msg();
					oob->timestamp = d_packet_rx_time; /* Relative time in microseconds */
					oob->length = d_pdu_len;
					oob->rssi = (char)(*iptr1 >> 8); /* dB Scale */
					oob->rate = d_packet_rate; /* Receive rate in units of 100 kBps */
					d_target_queue->insert_tail(msg);               // send it
					msg.reset();                            // free it up
					d_state = PLCP_STATE_SEARCH_PREAMBLE;
					d_shift = 0;
					d_rate = PLCP_RATE_1MBPS;
				}
				break;
		}
		++iptr1;
		++iptr2;
	}
	consume_each(noutput_items);
	d_symbol_count += (noutput_items << 3);
	return num_output;
}
int BBN_PLCP::ProcessByte(unsigned char *byte, unsigned char *seed) 
{
	int index;
	scramble_data_t *table_entry;
	if (d_table == NULL)
		return -1;
	*seed &= 0x7f;
	index = ((int)(*seed) << 8) | (*byte);
	table_entry = d_table + index;
	*byte = table_entry->data;
	*seed = table_entry->seed;
	return 0;
}
int BBN_PLCP::DescrambleBits(uchar *byte, uchar *seed, int num_bits) 
{
	uint data;
	uchar output;
	uint next_bit;
#if DESCRAMBLER_DEBUG  
	int k;
	int test;
#endif
	if ((num_bits > 8) || (num_bits < 0))
		return -1;
	output = 0;
	data = *byte;
	for (int j=0; j<num_bits; ++j) 
	{
#if DESCRAMBLER_DEBUG
		test = *seed;
		printf("%d: ", (data >> 7) & 0x01);
		for (k=0; k<7; ++k)
		{
			printf("%d ", (test >> 6) & 0x01);
			test <<= 1;
		}
#endif
		next_bit = ( (data >> 7) + (*seed >> 3) + (*seed) ) & 0x01;
		output = (output << 1) | (next_bit);
		*seed >>= 1;
		*seed += ((data >> 1) & 0x40);
		data <<= 1;
#if DESCRAMBLER_DEBUG
		printf("- %d    ", next_bit);
		if (j & 0x01)
			printf("\n");
#endif
	}
	*byte = output;
	return 0;
}

#define CCITT_CRC16_POLY (0x8408)
#define IEEE_802_3_CRC32_POLY (0xEDB88320)

void BBN_PLCP::CRC16Init(void) 
{
	if (crc_table_initialized == false) 
	{
		for (int i=0; i<256; ++i) 
		{
			uchar c = (uchar)i;
			crc16_table[i] = UpdateCRC(0, &c, 1, CCITT_CRC16_POLY);
			crc32_table[i] = UpdateCRC(0, &c, 1, IEEE_802_3_CRC32_POLY);
		}
		crc_table_initialized = true;
	}
}
uint BBN_PLCP::CRC32(const uchar *data, int len) 
{
	uint crc = 0xffffffff;
	if (crc_table_initialized == false)
		CRC16Init();
	for (int i=0; i<len; ++i)
		crc = (crc >> 8) ^ crc32_table[(crc ^ data[i]) & 0xff];
	return crc^0xffffffff;
}
uint BBN_PLCP::UpdateCRC(uint crc, const uchar *data, int len, uint poly) 
{
	ushort ch;
	for (int i=0; i<len; ++i) 
	{
		ch = data[i];
		for (int j=0; j<8; ++j) 
		{
#if CRC_DEBUG
			if (ch & 0x0001) 
				printf("1: ");
			else
				printf("0: ");
#endif
			if ((crc ^ ch) & 0x0001) 
				crc = (crc >> 1) ^ poly;
			else
				crc = (crc >> 1);
			ch >>= 1;
#if CRC_DEBUG
			for (int k=0; k<16; ++k) 
			{
				if(crc & (0x01 << k))
					printf("1");
				else
					printf("0");
			}
			printf("\n");
#endif
		}
  	}
#if CRC_DEBUG
	printf("   ");
	for (int k=0; k<16; ++k) 
	{
		if(~crc & (0x01 << k))
			printf("1");
		else
			printf("0");
	}
	printf("\n");
	printf("crc = %04X\n", crc);
#endif
	return crc;
} 
ushort BBN_PLCP::CRC16(const uchar *data, int len) 
{
	ushort crc = 0xffff;
	if (crc_table_initialized == false)
		CRC16Init();
	for (int i=0; i<len; ++i) 
	{
		crc = (crc >> 8) ^ crc16_table[(crc ^ data[i]) & 0xff];
#if CRC_DEBUG
		for (int k=0; k<16; ++k) 
		{
			if(crc & (0x01 << k))
				printf("1");
			else
				printf("0");
		}
		printf("\n");
#endif
	}
#if CRC_DEBUG
	printf("\n");
	printf("crc = %04X\n", crc);
#endif
	return crc^0xffff;
}
