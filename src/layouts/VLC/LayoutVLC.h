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
		std::vector<int> tabs;
		QWidget *CreateTabOpts(QWidget * = NULL);
		void init_v_VLC (VarVLC *, QWidget *);
		void ReadSettings();
			
	public:
		LayoutVLC(MainWindow *, int);
		const char *Name();
		static LayoutFactory::sptr Create(MainWindow *, int);
		VarVLC *varVLC;
		void Run();
		void Stop();
		
	public slots:
		void RadioPressed(bool);
		void SaveSettings(QSettings &);
		void setPSDUunits(int);
		void setFramesize(bool);
		void setDatarate(bool);
};

#endif //_INC_LAYOUTVLC_H_
