// $Id$
#ifndef _INC_LAYOUTVLC_H_
#define _INC_LAYOUTVLC_H_

#include "LayoutFactory.h"
#include "RxVLC.h"
#include "TxVLC.h"
#include "MainWindow.h"
#include <gr_top_block.h>
#include <QObject>
#include <QComboBox>
#include <QSettings>
#include <QTimer>
#include <QMutex>

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
	QCheckBox *ch_voip;
}VarVLC;

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
		void Run();
		void Stop();
		static unsigned int bi2dec(int *, unsigned int);
		static void dec2bi(unsigned int, unsigned int, int *);
		static unsigned short GenerateCRC(int *, int *, int);
		static bool CheckCRC(int *, int);
		static const int CRC_LENGTH = 16;
		QGridLayout *gridErrors;
		QGridLayout *gridLink;
		QGridLayout *gridMeas;
		void EmitChangeMetric(QLabel *, QString);
		unsigned int bits;
		QMutex mtx;
		
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
