#ifndef _INC_RX80211B_H_
#define _INC_RX80211B_H_

#include <gr_hier_block2.h>
#include <gr_msg_queue.h>
#include <QThread>

class Rx80211bThread : public QThread
{
	public:
		Rx80211bThread(gr_msg_queue_sptr);
	protected:
		void run();
	private:
		gr_msg_queue_sptr queue;
};
class Layout80211b;
class Rx80211b : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<Rx80211b> sptr;
		static sptr Create(Layout80211b *);
		~Rx80211b();
		static std::vector<float> BarkerTaps(int);
		void stop();
		
	private:
		Rx80211b(Layout80211b *);
		gr_msg_queue_sptr msgq;
		Rx80211bThread *rxth;
		static int sinc(int, float, float *);
		Layout80211b *ly;
};

#endif //_INC_RX80211B_H_
