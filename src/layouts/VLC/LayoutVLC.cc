// $Id$
#include "LayoutVLC.h"
#include <gr_uhd_usrp_source.h>
#include <gr_uhd_usrp_sink.h>
#include "RxVLC.h"
#include "TxVLC.h"
#include "MainWindow.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QRadioButton>
#include <iostream>

#include <gr_null_sink.h>
#include <gr_sig_source_f.h>
#include <gr_float_to_complex.h>
#include <gr_file_source.h>
#include "bbMatlab.h"

const char *LayoutVLC::name = "VLC";

double rate_phy1_o [] = { 
	11.67, 24.44, 48.89, 73.3, 100
};

double rate_phy1_v [] = {
	35.56, 71.11, 124.4, 266.6
};

double rate_phy2_v [] = {
	1.25, 2, 2.5, 4, 5
};

double rate_phy2_o [] = {
	6, 9.6, 12, 19.2, 24, 38.4, 48, 76.8, 96
};
	

LayoutVLC::LayoutVLC(MainWindow *_mw, int _radioID) :
	LayoutFactory(),mw(_mw), radioID(_radioID)
{
	QObject::connect(mw->panel->rb_layout[radioID]->bt, SIGNAL(toggled(bool)), this, SLOT(RadioPressed(bool)));
	QObject::connect(mw, SIGNAL(StateLayoutChanged(MainWindow::StatesLayout)), this, SLOT(StateLayout(MainWindow::StatesLayout)));
}
const char *LayoutVLC::Name()
{
	return name;
}
LayoutFactory::sptr LayoutVLC::Create(MainWindow *_mw, int _radioID)
{
	return LayoutFactory::sptr(new LayoutVLC(_mw, _radioID));
}

void LayoutVLC::Run()
{
	grTop = gr_make_top_block(std::string(name));
	QString addr = QString("addr0=%1").arg(mw->panel->ipfield[0].ip->text().remove(' '));
	for (int i = 1; i < mw->panel->sp_devs->value(); i++)
		addr.append(",addr%1=%2").arg(i).arg(mw->panel->ipfield[i].ip->text().remove(' '));
	if (mw->panel->rb_chain[RB_RX]->isChecked())
	{
		usrp_rx = uhd_make_usrp_source(addr.toStdString(), uhd::stream_args_t("fc32","sc16"));
		usrp_rx->set_samp_rate(800e3);
		usrp_rx->set_center_freq(0);
		usrp_rx->set_gain(mw->panel->sp_gain->value());
		//rx = RxVLC::Create();
		bbMatlab::sptr bbm = bbMatlab::Create("rx.txt", sizeof(std::complex<float>));
		grTop->connect(usrp_rx, 0, bbm, 0);
		grTop->start();
	}
	else //transmitter
	{
		/*usrp_tx = uhd_make_usrp_sink(addr.toStdString(), uhd::stream_args_t("fc32","sc8"));
		usrp_tx->set_samp_rate(800e3);
		usrp_tx->set_center_freq(0);
		usrp_tx->set_gain(mw->panel->sp_gain->value());
		//gr_sig_source_f_sptr sig = gr_make_sig_source_f(20e6, GR_SIN_WAVE, 200e3, 1, 0);
		gr_file_source_sptr sig = gr_make_file_source(sizeof(float), "src/layouts/VLC/input_data.txt.dat", true);
		gr_float_to_complex_sptr f2c = gr_make_float_to_complex();
		grTop->connect(sig, 0, f2c, 0);
		grTop->connect(f2c, 0, usrp_tx, 0);
		grTop->start();
		*/tx = TxVLC::Create(this);
		if (tx->vlc_var.phy_type)
		{
			printf("PHY II mode is not still available\n");
			exit(-1);
		}
		/*else
		{
			switch (tx->vlc_var.mod_type)
			{
				case 0:
				usrp_tx->set_samp_rate(200e3);	
				//other settings of USRP
				break;
				case 1:
				usrp_tx->set_samp_rate(800e3);
				//other settings of USRP, VPPM is OOK with twice the speed
				break;
			}
		}
		*/gr_null_sink_sptr sink = gr_make_null_sink(sizeof(float));		
		grTop->connect(tx, 0, sink, 0);
		grTop->start();
		
	}
}
void LayoutVLC::Stop()
{
	if (mw->panel->rb_chain[RB_RX]->isChecked())
		rx->stop();
	else
		tx->stop();
	grTop->stop();
	grTop->wait();
	//grTop.reset();
}
void LayoutVLC::RadioPressed(bool checked)
{
	if (checked)
	{
		mw->AddCustomTab(CreateTabOpts(), QString("Options"));
		mw->panel->rb_chain[RB_TX]->setHidden(false);
		mw->panel->rb_chain[RB_RX]->setHidden(false);
		mw->panel->rb_chain[RB_TX]->setChecked(true);
		ReadSettings(mw->s);
		QObject::connect(mw, SIGNAL(SaveSettings(QSettings *)), this, SLOT(SaveSettings(QSettings *)));
	}
	else
	{
		SaveSettings(mw->s);
		mw->RemoveCustomTabs();
		QObject::disconnect(mw, SIGNAL(SaveSettings(QSettings *)), this, SLOT(SaveSettings(QSettings *)));
	}
}

QWidget *LayoutVLC::CreateTabOpts()
{
	QWidget *p = new QWidget(mw);
	varVLC = new VarVLC;
	init_v_VLC(varVLC, p);
	QGroupBox *gBox = new QGroupBox(tr("General Settings"), p);
	QGroupBox *gBox_data = new QGroupBox(tr("Phy layer Settings"), p);
	QGridLayout *vBox = new QGridLayout;
	QGridLayout *vBox_data = new QGridLayout;
		
	vBox->addWidget(varVLC->l_tx_mode);
	vBox->addWidget(varVLC->cb_tx_mode);
	vBox->addWidget(varVLC->l_psdu_units);
	//vBox->addWidget(varVLC->sp_psdu_units);
	vBox->addWidget(varVLC->sp_psdu_units[0]);
	vBox->addWidget(varVLC->sp_psdu_units[1]);
	vBox->addWidget(varVLC->l_phy_type);
	vBox->addWidget(varVLC->rb_phy_type[0]);
	vBox->addWidget(varVLC->rb_phy_type[1]);
	vBox->addWidget(varVLC->l_flp_length);
	vBox->addWidget(varVLC->sp_flp_length);
	gBox->setLayout(vBox);
	vBox_data->addWidget(varVLC->l_phy_modulation);
	vBox_data->addWidget(varVLC->rb_phy_modulation[0]);
	vBox_data->addWidget(varVLC->rb_phy_modulation[1]);
	vBox_data->addWidget(varVLC->l_phy_op_mode);
	//vBox_data->addWidget(varVLC->cb_phy_op_mode);
	vBox_data->addWidget(varVLC->cb_phy_op_mode[0]);
	vBox_data->addWidget(varVLC->cb_phy_op_mode[1]);
	vBox_data->addWidget(varVLC->cb_phy_op_mode[2]);
	vBox_data->addWidget(varVLC->cb_phy_op_mode[3]);
	vBox_data->addWidget(varVLC->l_frame_size);
	//vBox_data->addWidget(varVLC->sp_frame_size);
	vBox_data->addWidget(varVLC->sp_frame_size[0]);
	vBox_data->addWidget(varVLC->sp_frame_size[1]);
	gBox_data->setLayout(vBox_data);
	QGridLayout *grid = new QGridLayout(p);
	grid->addWidget(gBox, 0, 0);
	grid->addWidget(gBox_data, 0,1);
	
	//connections
	QObject::connect(varVLC->cb_tx_mode, SIGNAL(currentIndexChanged (int)), this, SLOT(setPSDUunits(int)));
	for (int i=0; i<2;i++)//sizeof(varVLC->rb_phy_type)/sizeof(QRadioButton); i++)
	{
		QObject::connect(varVLC->rb_phy_type[i], SIGNAL(clicked(bool )), this, SLOT(setFramesize(bool )));
		QObject::connect(varVLC->rb_phy_type[i], SIGNAL(toggled(bool )), this, SLOT(setDatarate(bool )));
		QObject::connect(varVLC->rb_phy_modulation[i], SIGNAL(toggled(bool )), this, SLOT(setDatarate(bool )));
	}
	
	return p;
	
}

void LayoutVLC::init_v_VLC(VarVLC *varVLC, QWidget *p)
{
	uint i;
	//Tx.Mode
	varVLC->l_tx_mode= new QLabel("Tx Mode:");
	varVLC->cb_tx_mode = new QComboBox(p);
	varVLC->cb_tx_mode->addItem(tr("Single"));
	varVLC->cb_tx_mode->addItem(tr("Packed"));
	varVLC->cb_tx_mode->addItem(tr("Burst"));
	QObject::connect(varVLC->cb_tx_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(TrackChanges()));
	
	//PSDU_units
	varVLC->l_psdu_units= new QLabel("PSDU_units:");
	/*varVLC->sp_psdu_units = new QSpinBox(p);
	varVLC->sp_psdu_units->setRange(1,1);
	varVLC->sp_psdu_units->setSingleStep(1);*/
	
	varVLC->sp_psdu_units[0] = new QSpinBox(p);
	varVLC->sp_psdu_units[0]->setRange(1,1);
	varVLC->sp_psdu_units[0]->setSingleStep(1);
	varVLC->sp_psdu_units[0]->setHidden(false);
	QObject::connect(varVLC->sp_psdu_units[0], SIGNAL(valueChanged(int)), this, SLOT(TrackChanges()));
	
	varVLC->sp_psdu_units[1] = new QSpinBox(p);
	varVLC->sp_psdu_units[1]->setRange(2,10);
	varVLC->sp_psdu_units[1]->setSingleStep(1);
	varVLC->sp_psdu_units[1]->setHidden(true);
	QObject::connect(varVLC->sp_psdu_units[1], SIGNAL(valueChanged(int)), this, SLOT(TrackChanges()));
	
	//Phy_type
	varVLC->l_phy_type = new QLabel("Phy Type:");
	varVLC->rb_phy_type[0] = new QRadioButton;
	varVLC->rb_phy_type[0]->setText(tr("PHY I"));
	varVLC->rb_phy_type[0]->setChecked(true);
	QObject::connect(varVLC->rb_phy_type[0], SIGNAL(toggled(bool)), this, SLOT(TrackChanges()));
	varVLC->rb_phy_type[1] = new QRadioButton;
	varVLC->rb_phy_type[1]->setText(tr("PHY II"));
	QObject::connect(varVLC->rb_phy_type[1], SIGNAL(toggled(bool)), this, SLOT(TrackChanges()));
	
	//FLP_length
	varVLC->l_flp_length= new QLabel("FLP length:");
	varVLC->sp_flp_length= new QSpinBox(p);
	varVLC->sp_flp_length->setRange(64,16384);
	varVLC->sp_flp_length->setSingleStep(100);
	varVLC->sp_flp_length->setHidden(false);
	QObject::connect(varVLC->sp_flp_length, SIGNAL(valueChanged(int)), this, SLOT(TrackChanges()));
		
	//Modulation
	varVLC->l_phy_modulation = new QLabel("Modulation:");
	varVLC->rb_phy_modulation[0] = new QRadioButton;
	varVLC->rb_phy_modulation[0]->setText(tr("OOK"));
	varVLC->rb_phy_modulation[0]->setChecked(true);
	QObject::connect(varVLC->rb_phy_modulation[0], SIGNAL(toggled(bool)), this, SLOT(TrackChanges()));
	varVLC->rb_phy_modulation[1] = new QRadioButton;
	varVLC->rb_phy_modulation[1]->setText(tr("VPPM"));
	QObject::connect(varVLC->rb_phy_modulation[1], SIGNAL(toggled(bool)), this, SLOT(TrackChanges()));
	
	//Operating mode
	varVLC->l_phy_op_mode = new QLabel("Phy operating mode:");
	//varVLC->cb_phy_op_mode =new QComboBox(p);
	//for (i = 0; i < sizeof(rate_phy1_o)/sizeof(double); i++)
		//varVLC->cb_phy_op_mode->addItem(QString("%1: %2 Kb/s").arg(i+1).arg(rate_phy1_o[i]));
	
	varVLC->cb_phy_op_mode[0] = new QComboBox(p);
	for (i = 0; i < sizeof(rate_phy1_o)/sizeof(double); i++)
		varVLC->cb_phy_op_mode[0]->addItem(QString("%1: %2 Kb/s").arg(i+1).arg(rate_phy1_o[i]));
	varVLC->cb_phy_op_mode[1] = new QComboBox(p);
	for (i = 0; i < sizeof(rate_phy1_v)/sizeof(double); i++)
		varVLC->cb_phy_op_mode[1]->addItem(QString("%1: %2 Kb/s").arg(i+1).arg(rate_phy1_v[i]));
	varVLC->cb_phy_op_mode[2] = new QComboBox(p);
	for (i = 0; i < sizeof(rate_phy2_o)/sizeof(double); i++)
		varVLC->cb_phy_op_mode[2]->addItem(QString("%1: %2 Mb/s").arg(i+1).arg(rate_phy2_o[i]));
	varVLC->cb_phy_op_mode[3] = new QComboBox(p);
	for (i = 0; i < sizeof(rate_phy2_v)/sizeof(double); i++)
		varVLC->cb_phy_op_mode[3]->addItem(QString("%1: %2 Mb/s").arg(i+1).arg(rate_phy2_v[i]));
	
	varVLC->cb_phy_op_mode[0]->setHidden(false);
	QObject::connect(varVLC->cb_phy_op_mode[0], SIGNAL(currentIndexChanged(int)), this, SLOT(TrackChanges()));
	varVLC->cb_phy_op_mode[1]->setHidden(true);
	QObject::connect(varVLC->cb_phy_op_mode[1], SIGNAL(currentIndexChanged(int)), this, SLOT(TrackChanges()));
	varVLC->cb_phy_op_mode[2]->setHidden(true);
	QObject::connect(varVLC->cb_phy_op_mode[2], SIGNAL(currentIndexChanged(int)), this, SLOT(TrackChanges()));
	varVLC->cb_phy_op_mode[3]->setHidden(true);
	QObject::connect(varVLC->cb_phy_op_mode[3], SIGNAL(currentIndexChanged(int)), this, SLOT(TrackChanges()));
	
	//frame_size
	varVLC->l_frame_size = new QLabel("Frame size (bytes):");
	/*varVLC->sp_frame_size = new QSpinBox(p);
	varVLC->sp_frame_size->setRange(0,1023);
	varVLC->sp_frame_size->setSingleStep(100);*/
	
	
	varVLC->sp_frame_size[0] = new QSpinBox(p);
	varVLC->sp_frame_size[0]->setRange(0,1023);
	varVLC->sp_frame_size[0]->setSingleStep(100);
	varVLC->sp_frame_size[0]->setHidden(false);
	QObject::connect(varVLC->sp_frame_size[0], SIGNAL(valueChanged(int)), this, SLOT(TrackChanges()));
	
	varVLC->sp_frame_size[1] = new QSpinBox(p);
	varVLC->sp_frame_size[1]->setRange(0,65535);
	varVLC->sp_frame_size[1]->setSingleStep(500);
	varVLC->sp_frame_size[1]->setHidden(true);
	QObject::connect(varVLC->sp_frame_size[1], SIGNAL(valueChanged(int)), this, SLOT(TrackChanges()));
}

void LayoutVLC::SaveSettings(QSettings *s)
{
	s->setValue("VLC/tx_mode", varVLC->cb_tx_mode->currentIndex());
	//s->setValue("VLC/psdu_units_0", varVLC->sp_psdu_units->value());
	s->setValue("VLC/psdu_units_0", varVLC->sp_psdu_units[0]->value());
	s->setValue("VLC/psdu_units_1", varVLC->sp_psdu_units[1]->value());
	
	for (int i = 0; i < 2; i++)
	{
		if (varVLC->rb_phy_type[i]->isChecked())
			s->setValue("VLC/phy_type", i);
		if (varVLC->rb_phy_modulation[i]->isChecked())
			s->setValue("VLC/modulation", i);
	}
	s->setValue("VLC/flp_length", varVLC->sp_flp_length->value());
	
	//s->setValue("VLC/phy_op_mode_0", varVLC->cb_phy_op_mode->currentIndex());
	//s->setValue("VLC/frame_size_0", varVLC->sp_frame_size->value());
	s->setValue("VLC/phy_op_mode_0", varVLC->cb_phy_op_mode[0]->currentIndex());
	s->setValue("VLC/phy_op_mode_1", varVLC->cb_phy_op_mode[1]->currentIndex());
	s->setValue("VLC/phy_op_mode_2", varVLC->cb_phy_op_mode[2]->currentIndex());
	s->setValue("VLC/phy_op_mode_3", varVLC->cb_phy_op_mode[3]->currentIndex());
	s->setValue("VLC/frame_size_0", varVLC->sp_frame_size[0]->value());
	s->setValue("VLC/frame_size_1", varVLC->sp_frame_size[1]->value());
}
void LayoutVLC::ReadSettings(QSettings *s)
{
	varVLC->cb_tx_mode->setCurrentIndex(s->value("VLC/tx_mode").toInt());
	//varVLC->sp_psdu_units->setValue(s->value("VLC/psdu_units_0",1).toInt());
	varVLC->sp_psdu_units[0]->setValue(s->value("VLC/psdu_units_0",1).toInt());
	varVLC->sp_psdu_units[1]->setValue(s->value("VLC/psdu_units_1",1).toInt());
	
	varVLC->rb_phy_type[s->value("VLC/phy_type", 0).toInt()]->setChecked(true);
	varVLC->rb_phy_modulation[s->value("VLC/modulation", 0).toInt()]->setChecked(true);
	
	varVLC->sp_flp_length->setValue(s->value("VLC/flp_length",1).toInt());
	
	//varVLC->cb_phy_op_mode->setCurrentIndex(s->value("VLC/tx_phy_op_mode_0").toInt());
	varVLC->cb_phy_op_mode[0]->setCurrentIndex(s->value("VLC/phy_op_mode_0").toInt());
	varVLC->cb_phy_op_mode[1]->setCurrentIndex(s->value("VLC/phy_op_mode_1").toInt());
	varVLC->cb_phy_op_mode[2]->setCurrentIndex(s->value("VLC/phy_op_mode_2").toInt());
	varVLC->cb_phy_op_mode[3]->setCurrentIndex(s->value("VLC/phy_op_mode_3").toInt());
		
	//varVLC->sp_frame_size->setValue(s->value("VLC/frame_size_0",1).toInt());
	varVLC->sp_frame_size[0]->setValue(s->value("VLC/frame_size_0",1).toInt());
	varVLC->sp_frame_size[1]->setValue(s->value("VLC/frame_size_1",1).toInt());
}

void LayoutVLC::setPSDUunits(int index)
{
	if (index!=0)
	{
		varVLC->sp_psdu_units[0]->setHidden(true);
		varVLC->sp_psdu_units[1]->setHidden(false);		
	}
	else
	{
		varVLC->sp_psdu_units[0]->setHidden(false);
		varVLC->sp_psdu_units[1]->setHidden(true);		
	}

}

void LayoutVLC::setFramesize(bool a)
{
	int checked;
	for (uint i=0; i<2; i++)
	{
		if (varVLC->rb_phy_type[i]->isChecked())
		{
			checked = i;
		}
	}
	switch (checked)
	{
		case 0:
			varVLC->sp_frame_size[0]->setHidden(false);
			varVLC->sp_frame_size[1]->setHidden(true);	
		break;
		case 1:
			varVLC->sp_frame_size[0]->setHidden(true);
			varVLC->sp_frame_size[1]->setHidden(false);	
		break;
	}
}

void LayoutVLC::setDatarate(bool a)
{
	int checked1,checked2;
	for (uint i=0; i<2; i++)
	{
		if (varVLC->rb_phy_type[i]->isChecked())
		{
			checked1 = i;
		}
		if (varVLC->rb_phy_modulation[i]->isChecked())
		{
			checked2 = i;
		}
	}
	switch (checked1)
	{
		case 0:
			switch (checked2)
			{
				case 0:
					varVLC->cb_phy_op_mode[0]->setHidden(false);
					varVLC->cb_phy_op_mode[1]->setHidden(true);
					varVLC->cb_phy_op_mode[2]->setHidden(true);
					varVLC->cb_phy_op_mode[3]->setHidden(true);
				break;
				case 1:
					varVLC->cb_phy_op_mode[0]->setHidden(true);
					varVLC->cb_phy_op_mode[1]->setHidden(false);
					varVLC->cb_phy_op_mode[2]->setHidden(true);
					varVLC->cb_phy_op_mode[3]->setHidden(true);
				break;
			}
			
		break;
		case 1:
			switch (checked2)
			{
				case 0:
					varVLC->cb_phy_op_mode[0]->setHidden(true);
					varVLC->cb_phy_op_mode[1]->setHidden(true);
					varVLC->cb_phy_op_mode[2]->setHidden(false);
					varVLC->cb_phy_op_mode[3]->setHidden(true);
				break;
				case 1:
					
					varVLC->cb_phy_op_mode[0]->setHidden(true);
					varVLC->cb_phy_op_mode[1]->setHidden(true);
					varVLC->cb_phy_op_mode[2]->setHidden(true);
					varVLC->cb_phy_op_mode[3]->setHidden(false);
				break;
			}
		break;
	}
	
}
void LayoutVLC::TrackChanges()
{
	if (tx)
		tx->UIChanged();	
}
unsigned char LayoutVLC::bi2dec(int *in, unsigned int GF)
{
	unsigned char o = 0;
	for (unsigned int i = 0; i < GF; i++)
		o = (o<<1|*in++);
	return o;
}
void LayoutVLC::dec2bi(int number, unsigned int GF, int *bin_number)
{
	//again the same criteria as in the rs-encoder 'left-msb'	
	for (int i=GF-1; i >= 0; i--)
		*bin_number++ = (number >> i) & 0x1;
    return;       
}
void LayoutVLC::StateLayout(MainWindow::StatesLayout s)
{
	if (s == MainWindow::STOPPING)
		Stop();
}
