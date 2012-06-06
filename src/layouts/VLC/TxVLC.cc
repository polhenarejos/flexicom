#include "compat.h"
#include "TxVLC.h"
#include <gr_io_signature.h>
#include <vector>
#include <QtGlobal>
#include <iostream>

/*TxVLCThread::TxVLCThread(gr_msg_queue_sptr _queue) :
	queue(_queue)
{
}

void TxVLCThread::run()
{
	while (1)
	{
		gr_message_sptr mesg = queue->delete_head();
		if (mesg)
		{
			printf("Hola caracola\n");
			
			
		}
		else
			sleep(1);
	}
}*/

TxVLC::TxVLC() :
	gr_hier_block2("TxVLC", gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, 1, sizeof(gr_complex)))
{
	//msgq = gr_make_msg_queue();
	//rxth = new TxVLCThread(msgq);
	//rxth->start();
	//connect(self(), 0, self(), 0); //dummy connection
	
}
TxVLC::sptr TxVLC::Create()
{
	return sptr(new TxVLC());
}
TxVLC::~TxVLC()
{
	//stop();
}

/*void TxVLC::stop()
{
	if (rxth)
	{
		rxth->terminate();
		rxth->wait();
		delete rxth;
		rxth = NULL;
	}
}*/

