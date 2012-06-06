#ifndef _INC_RXVLC_H_
#define _INC_RXVLC_H_

#include <gr_hier_block2.h>
#include <gr_msg_queue.h>
#include <QThread>

class RxVLCThread : public QThread
{
	public:
		RxVLCThread(gr_msg_queue_sptr);
	protected:
		void run();
	private:
		gr_msg_queue_sptr queue;
		//this is the class where we process the received information, after demodulation
};
class RxVLC : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<RxVLC> sptr;
		static sptr Create();
		~RxVLC();
		void stop();
		
	private:
		RxVLC();
		gr_msg_queue_sptr msgq;
		RxVLCThread *rxth;
};

#endif //_INC_RXVLC_H_
