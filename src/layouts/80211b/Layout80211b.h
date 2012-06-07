#ifndef _INC_LAYOUT80211B_H_
#define _INC_LAYOUT80211B_H_

#include "LayoutFactory.h"
#include "Rx80211b.h"
#include <qwt_plot.h>
#include <gr_top_block.h>
#include <QObject>
#include <QComboBox>
#include <QSettings>
class uhd_usrp_source;
class MainWindow;

class Layout80211b : public QObject, public LayoutFactory
{
	Q_OBJECT 
	
	private:
		static const char *name;
		gr_top_block_sptr grTop;
		Rx80211b::sptr rx;
		//Tx80211b::sptr tx;
		boost::shared_ptr<uhd_usrp_source> usrp;
		MainWindow *mw;
		int radioID;
		std::vector<int> tabs;
		QWidget *CreateTabOpts(QWidget * = NULL);
		QComboBox *cb_chans;
		void ReadSettings();
		void DrawPlots();
		
	public:
		Layout80211b(MainWindow *, int);
		const char *Name();
		static LayoutFactory::sptr Create(MainWindow *, int);
		void Run();
		void Stop();
		QwtPlot *pl_osc;
		
	public slots:
		void RadioPressed(bool);
		void SaveSettings(QSettings &);
};

#endif //_INC_LAYOUT80211B_H_
