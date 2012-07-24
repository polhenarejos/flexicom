// $Id$
#include "bb_Header_cp.h"
#include "vlc_crc.h"
#include <gr_io_signature.h>
#include <gr_msg_queue.h>
#include <math.h>


typedef unsigned int uint;

bb_Header_cp::~bb_Header_cp()
{
	if (crc_cp)
	{
		delete crc_cp;
		crc_cp = 0;
	}
}

bb_Header_cp::bb_Header_cp(int _flag, int _raw_length, gr_msg_queue_sptr _d_queue):
	gr_block("bb_Header_cp", gr_make_io_signature (1,1, sizeof(int)), gr_make_io_signature (0,0, sizeof(int))),
	flag(_flag), raw_length(_raw_length), d_queue(_d_queue)
{
		crc_cp= new vlc_crc();
}


bb_Header_cp::sptr bb_Header_cp::Create(int _flag, int _raw_length, gr_msg_queue_sptr _d_queue)
{
	return sptr(new bb_Header_cp(_flag, _raw_length, _d_queue));
}

void bb_Header_cp::forecast(int noutput_items, gr_vector_int &ninput_items_required) 
{
	uint ninputs = ninput_items_required.size();
	for (uint i=0; i < ninputs; i++)
		ninput_items_required[i] = (noutput_items/16)* raw_length; //16 = length of the crc
}

int bb_Header_cp::general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) 
{
	int *iptr= (int *)input_items[0];
	int blocks_to_process,i, ones, zeros;
	int *tmp;
	bool check;
	int n_output =0;
	int length;
	blocks_to_process= (noutput_items/16);
	for (i=0; i<blocks_to_process; i++)
	{
		if (flag==0) //PHR
		{
			length = 32+16+16;
			tmp = new int [length]; //the important bits of the PHR are the first 48
			memset(tmp,0,sizeof(int)*length);
			memcpy(tmp,iptr, sizeof(int)*48);
			
		}
		else //PSDU
		{
			length = raw_length + 16;
			tmp = new int [length];			
			memset(tmp,0,sizeof(int)*length);
			memcpy(tmp,iptr, sizeof(int)*raw_length);
		}
		iptr = iptr + raw_length;
		check = crc_cp->check_crc(tmp, &tmp[length-16],length);
		if (check == true) //crc ok!!
		{
			if (flag==0)
			{
				gr_message_sptr msg = gr_make_message(0, 0, 0, sizeof(int)*(32+1));
				memcpy(msg->msg(), &flag, sizeof(int)*1);
				memcpy(msg->msg() + sizeof(int), tmp, sizeof(int)*32);
				d_queue->insert_tail(msg);
				msg.reset();
			}
			else
			{
				gr_message_sptr msg = gr_make_message(0, 0, 0, sizeof(int)*(40+1));
				memcpy(msg->msg(), &flag, sizeof(int)*1);
				memcpy(msg->msg() + sizeof(int), tmp, sizeof(int)*40);
				d_queue->insert_tail(msg);  //send it
				msg.reset(); //free it up
			}
			
		}
		blocks_to_process--;
	}
	consume_each(raw_length*blocks_to_process);
	return n_output;
}
