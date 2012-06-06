#ifndef _INC_LAYOUTVLC_H_
#define _INC_LAYOUTVLC_H_

#include "LayoutFactory.h"
#include "RxVLC.h"
#include "TxVLC.h"
#include <gr_top_block.h>
#include <QObject>
class uhd_usrp_source;
class MainWindow;

class LayoutVLC : public QObject , public LayoutFactory
{
	Q_OBJECT
	
	private:
		static const char *name;
		gr_top_block_sptr grTop;
		RxVLC::sptr rx;
		TxVLC::sptr tx;
		boost::shared_ptr<uhd_usrp_source> usrp;
		MainWindow *mw;
		int radioID;
	public:
		LayoutVLC(MainWindow *, int);
		const char *Name();
		static LayoutFactory::sptr Create(MainWindow *, int);
		void Run();
		void Stop();
		
	public slots:
		void RadioPressed(bool);
};

#endif //_INC_LAYOUTVLC_H_
