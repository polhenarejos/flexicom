// $Id$
#ifndef _INC_LAYOUT80211B_H_
#define _INC_LAYOUT80211B_H_

#include "LayoutFactory.h"
#include "Rx80211b.h"
#include <qwt_plot.h>
#include <gnuradio/top_block.h>
#include <QtCore/QObject>
#include <QtWidgets/QComboBox>
#include <QtCore/QSettings>
#include <gnuradio/uhd/usrp_source.h>
class MainWindow;

class Layout80211b : public QObject, public LayoutFactory
{
	Q_OBJECT 
	
	private:
		static const char *name;
		gr::top_block_sptr grTop;
		Rx80211b::sptr rx;
		//Tx80211b::sptr tx;
		gr::uhd::usrp_source::sptr usrp;
		QWidget *CreateTabOpts();
		QComboBox *cb_chans;
		void ReadSettings(QSettings *);
		void DrawPlots();
		void RemovePlots();
		
	public:
		Layout80211b(MainWindow *, int);
		const char *Name();
		static LayoutFactory::sptr Create(MainWindow *, int);
		void Run();
		void Stop();
		QwtPlot *pl_osc;
		
	public slots:
		void RadioPressed(bool);
		void SaveSettings(QSettings *);
};

#endif //_INC_LAYOUT80211B_H_
