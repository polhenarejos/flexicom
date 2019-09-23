// $Id$
#ifndef _INC_TXVLC_H_
#define _INC_TXVLC_H_

#include <gnuradio/hier_block2.h>
#include <boost/thread.hpp>
#include "DataSource.h"

//#include <gnuradio/msg_queue.h>
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

class LayoutVLC;

/*! \brief The class where the layout of the VLC communication system (transmitter part) is defined

* In this class, the TXVLC or the RXVLC is connected to the usrp class object to stablish the stream of samples
* Another functionality of this class is to read the tab which configures the VLC system.

*/

class TxVLC : public gr::hier_block2
{
	public:
		typedef boost::shared_ptr<TxVLC> sptr;
	   /**
       * The creation of the TxVLC block requires 1 parameter: 
       * @param _ly: VLC layout, where the transmitter is connected to the usrp object. 
       */
		static sptr Create(LayoutVLC *);
		~TxVLC();
		void stop();
		void UIChanged();
		uint64_t GetConfigVer();
		void UpdateConfigVer();
		int SendPacket(unsigned char *, unsigned int);
		
	private:
		TxVLC(LayoutVLC *);
		LayoutVLC *ly;
		void init_var();
		boost::mutex mutex;
		DataSource::sptr data_source;
};

#endif //_INC_TXVLC_H_
