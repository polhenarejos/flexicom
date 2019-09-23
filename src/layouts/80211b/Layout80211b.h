/* 
 * This file is part of the FlexiCom distribution (https://github.com/polhenarejos/flexicom).
 * Copyright (c) 2012-2020 Pol Henarejos, at Centre Tecnologic de Telecomunicacions de Catalunya (CTTC).
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/* $Id$ */
/* $Format:%ci$ ($Format:%h$) by $Format:%an%$ */

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
