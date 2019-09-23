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

#ifndef _INC_LAYOUTVLC_H_
#define _INC_LAYOUTVLC_H_

#include "LayoutFactory.h"
#include "RxVLC.h"
#include "TxVLC.h"
#include "MainWindow.h"
#include <gnuradio/top_block.h>
#include <QtCore/QObject>
#include <QtWidgets/QComboBox>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtCore/QMutex>
#include "compat.h"

PACK(
struct
{
	unsigned B:1;
	unsigned CN:3;
	unsigned MCS:6;
	unsigned PL:16;
	unsigned DO:1;
	unsigned rvd:5;
} , PHYHdr );

class uhd_usrp_source;
class uhd_usrp_sink;
class MainWindow;
class QRadioButton;
class QPushButton;
class QLineEdit;
class QSpinBox;
class QComboBox;
class QLabel;
class QTextEdit;

typedef struct
{
	QLabel *l_tx_mode;
	QComboBox *cb_tx_mode;
	
	QLabel *l_psdu_units;
	QSpinBox *sp_psdu_units[2];
	
	QLabel *l_phy_type;
	QRadioButton *rb_phy_type[2];
	
	QLabel *l_flp_length;
	QSpinBox *sp_flp_length;
	
	QLabel *l_phy_modulation;
	QRadioButton *rb_phy_modulation	[2];
			
	QLabel *l_phy_op_mode;
	QComboBox *cb_phy_op_mode[4];
	
	QLabel *l_frame_size; //bytes
	QSpinBox *sp_frame_size[2];
	
	QTextEdit *tx_chat;
	QLineEdit *le_chat;
	QPushButton *pb_chat;
	QCheckBox *ch_media;
}VarVLC;

//For simplicity, a struct which contains all the variables is created


/*! \brief The class where the layout of the VLC communication system is defined

* In this class, the TXVLC or the RXVLC is connected to the usrp class object to stablish the stream of samples
* Another functionality of this class is to manage the tab which configures the VLC system.

*/

class LayoutVLC : public QObject , public LayoutFactory
{
	Q_OBJECT
	
	private:
		static const char *name;
		gr::top_block_sptr grTop;
		RxVLC::sptr rx;
		TxVLC::sptr tx;
		QWidget *CreateTabOpts();
		QWidget *CreateTabChat();
		QWidget *CreateTabMetrics();
		void init_v_VLC (VarVLC *, QWidget *);
		void ReadSettings(QSettings *);
		QTimer *timer;
		unsigned int secs;

	public:
		LayoutVLC(MainWindow *, int);
		const char *Name();
		static LayoutFactory::sptr Create(MainWindow *, int);
		VarVLC *varVLC;
		typedef enum { OOK = 0, VPPM = 1 } Modulation;
		typedef enum { PHY_I = 0, PHY_II = 1 } PHYType;
		typedef enum { SINGLE = 0, PACKED = 1, BURST = 2 } TXMode;
		struct
		{
			PHYType phy_type;
			Modulation mod_type;
			int clock_rate;
			TXMode tx_mode;
			unsigned int psdu_units;
			int operating_mode;
			unsigned int frame_size;
			unsigned int PHR_raw_length; //raw length prior to modulation
			unsigned int PSDU_raw_length; //raw length prior to modulation
			int MCSID[6]; //field for the PHR preamble
			int flp_length;
			uint64_t count;
			unsigned int dMCSID;
		}vlc_var;
		void Run();
		void Stop();
		static unsigned short GenerateCRC(int *, int *, int);
		static bool CheckCRC(int *, int);
		static const int CRC_LENGTH = 16;
		QGridLayout *gridErrors;
		QGridLayout *gridLink;
		QGridLayout *gridMeas;
		void EmitChangeMetric(QLabel *, QString);
		unsigned int bits;
		unsigned int syncs;
		QMutex mtx;
		void SendReport();
		static int GetModulatedResources(PHYType, Modulation, int, int); //returns the number of bits after modulation
		
	public slots:
		void RadioPressed(bool);
		void SaveSettings(QSettings *);
		void setPSDUunits(int);
		void setFramesize(bool);
		void setDatarate(bool);
		void TrackChanges();
		void StateLayout(MainWindow::StatesLayout);
		void ChatClicked();
		void ChatText(QString &);
		void ChatAppend(QString &);
		void ChangedMetric(QLabel *, QString);
		void UpdateSpeed();
		
	signals:
		void ChatSigText(QString &);
		void ChangeMetric(QLabel *, QString);
};

#endif //_INC_LAYOUTVLC_H_
