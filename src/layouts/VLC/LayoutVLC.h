#ifndef _INC_LAYOUTVLC_H_
#define _INC_LAYOUTVLC_H_

#include "LayoutFactory.h"
#include "RxVLC.h"
#include "TxVLC.h"
#include "VarVLC.h"
#include <gr_top_block.h>
#include <QObject>
#include <QComboBox>
#include <QSettings>

class uhd_usrp_source;
class uhd_usrp_sink;
class MainWindow;

/*! \brief The class where the layout of the VLC communication system is defined

* In this class, the TXVLC or the RXVLC is connected to the usrp class object to stablish the stream of samples
* Another functionality of this class is to manage the tab which configures the VLC system.

*/

class LayoutVLC : public QObject , public LayoutFactory
{
	Q_OBJECT
	
	private:
		static const char *name;
		gr_top_block_sptr grTop;
		RxVLC::sptr rx;
		TxVLC::sptr tx;
		boost::shared_ptr<uhd_usrp_source> usrp_rx;
		boost::shared_ptr<uhd_usrp_sink> usrp_tx;
		MainWindow *mw;
		int radioID;
		QWidget *CreateTabOpts();
		void init_v_VLC (VarVLC *, QWidget *);
		void ReadSettings(QSettings &);
			
	public:
		LayoutVLC(MainWindow *, int);
		const char *Name();
		static LayoutFactory::sptr Create(MainWindow *, int);
		VarVLC *varVLC;
		void Run();
		void Stop();
		static unsigned char bi2dec(int *, unsigned int);
		
	public slots:
		void RadioPressed(bool);
		void SaveSettings(QSettings &);
		void setPSDUunits(int);
		void setFramesize(bool);
		void setDatarate(bool);
		void TrackChanges();
};

#endif //_INC_LAYOUTVLC_H_
