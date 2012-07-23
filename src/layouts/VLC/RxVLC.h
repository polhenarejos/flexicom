// $Id$
#ifndef _INC_RXVLC_H_
#define _INC_RXVLC_H_

#include <gr_hier_block2.h>
#include <gr_msg_queue.h>
#include <QThread>
#include "TxVLC.h"

/*! \brief The class where we process the received PHR information, after demodulation
*/
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


class LayoutVLC;

/*! \brief The class where the layout of the VLC communication system (receiver part) is defined

* In this class, the TXVLC or the RXVLC is connected to the usrp class object to stablish the stream of samples
* Another functionality of this class is to read the tab which configures the VLC system.

*/

class RxVLC : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<RxVLC> sptr;
		static sptr Create(LayoutVLC *);
		~RxVLC();
		void stop();
		VLCvar vlc_var_rx;
		
	private:
		int poly[3];
		RxVLC(LayoutVLC *);
		LayoutVLC *ly;
		void init_var();
		int get_modulated_resources(int,int,int,int,int,int,int,int); //returns the number of bits after modulation
		unsigned int PHR_modulated_length; //length of the PHR after all the modulating chain
		unsigned int PSDU_modulated_length; //length of the PSDU after all the modulating chain
		gr_msg_queue_sptr msgq;
		RxVLCThread *rxth;
};

#endif //_INC_RXVLC_H_
