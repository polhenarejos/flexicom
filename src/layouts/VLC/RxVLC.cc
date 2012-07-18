// $Id$
#include "compat.h"
#include "RxVLC.h"
#include <gr_io_signature.h>
#include <vector>
#include <QtGlobal>
#include <iostream>

RxVLCThread::RxVLCThread(gr_msg_queue_sptr _queue) :
	queue(_queue)
{
}

void RxVLCThread::run()
{
	while (1)
	{
		gr_message_sptr mesg = queue->delete_head();
		if (mesg)
		{
			//printf("Hola caracola\n");
			
			
		}
		else
			sleep(1);
	}
}

RxVLC::RxVLC() :
	gr_hier_block2("RxVLC", gr_make_io_signature(1, 1, sizeof(gr_complex)), gr_make_io_signature(0, 0, 0))
{
	msgq = gr_make_msg_queue();
	rxth = new RxVLCThread(msgq);
	rxth->start();
	connect(self(), 0, self(), 0); //dummy connection
	
}
RxVLC::sptr RxVLC::Create()
{
	return gnuradio::get_initial_sptr(new RxVLC());
}
RxVLC::~RxVLC()
{
	stop();
}
void RxVLC::stop()
{
	if (rxth)
	{
		rxth->terminate();
		rxth->wait();
		delete rxth;
		rxth = NULL;
	}
}

