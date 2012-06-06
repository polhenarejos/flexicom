#ifndef _INC_TXVLC_H_
#define _INC_TXVLC_H_

#include <gr_hier_block2.h>
#include <gr_msg_queue.h>
#include <QThread>

/*class TxVLCThread : public QThread
{
	public:
		TxVLCThread(gr_msg_queue_sptr);
	protected:
		void run();
	private:
		gr_msg_queue_sptr queue;
};*/
class TxVLC : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<TxVLC> sptr;
		static sptr Create();
		~TxVLC();
		//void stop();
		
	private:
		TxVLC();
		//gr_msg_queue_sptr msgq;
		//TxVLCThread *txth;
};

#endif //_INC_TXVLC_H_
