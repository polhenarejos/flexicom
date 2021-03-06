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

#include "LayoutVLC.h"
#include "RxVLC.h"
#include "TxVLC.h"
#include "MainWindow.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QRadioButton>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QTextEdit>
#include <iostream>

const char *LayoutVLC::name = "VLC";
static int _d_ = RegisterLayout(LayoutVLC::Create);

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
	LayoutFactory(_mw, _radioID)
{
	QObject::connect(mw->panel->rb_layout[radioID]->bt, SIGNAL(toggled(bool)), this, SLOT(RadioPressed(bool)));
	QObject::connect(mw, SIGNAL(StateLayoutChanged(MainWindow::StatesLayout)), this, SLOT(StateLayout(MainWindow::StatesLayout)));
	QObject::connect(this, SIGNAL(ChatSigText(QString &)), this, SLOT(ChatText(QString &)));
	QObject::connect(this, SIGNAL(ChangeMetric(QLabel *, QString)), this, SLOT(ChangedMetric(QLabel *, QString)));
	qRegisterMetaType<QString>("QString &");
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(UpdateSpeed()));
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
	grTop = gr::make_top_block(std::string(name));
	if (mw->panel->rb_chain[RB_RX]->isChecked())
	{
		rx = RxVLC::Create(this);
		grTop->connect(Source(), 0, rx, 0);
	}
	else //transmitter
	{
		varVLC->le_chat->setEnabled(true);
		varVLC->pb_chat->setEnabled(true);
		tx = TxVLC::Create(this);
		grTop->connect(tx, 0, Sink(), 0);
	}
	secs = bits = syncs = 0;
	timer->start(1000);
	grTop->start();
}
void LayoutVLC::Stop()
{
	timer->stop();
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
		mw->AddCustomTab(CreateTabChat(), QString("Chat"));
		mw->panel->rb_chain[RB_TX]->setHidden(false);
		mw->panel->rb_chain[RB_RX]->setHidden(false);
		//mw->panel->rb_chain[RB_TX]->setChecked(false);
		ReadSettings(mw->s);
		QObject::connect(mw, SIGNAL(SaveSettings(QSettings *)), this, SLOT(SaveSettings(QSettings *)));		
		mw->plots->addTab(CreateTabMetrics(), QString("Metrics"));
	}
	else
	{
		//SaveSettings(mw->s);
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
	grid->addWidget(varVLC->ch_media, 1, 0, 2, 1);
	
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
QWidget *LayoutVLC::CreateTabChat()
{
	QWidget *p = new QWidget(mw);
	QGridLayout *grid = new QGridLayout(p);
	varVLC->tx_chat = new QTextEdit(p);
	varVLC->le_chat = new QLineEdit(p);
	varVLC->pb_chat = new QPushButton(tr("Send"));
	grid->addWidget(varVLC->tx_chat, 0, 0, 1, 2);
	grid->addWidget(varVLC->le_chat, 1, 0);
	grid->addWidget(varVLC->pb_chat, 1, 1);
	QObject::connect(varVLC->pb_chat, SIGNAL(clicked()), this, SLOT(ChatClicked()));
	QObject::connect(varVLC->le_chat, SIGNAL(returnPressed()), this, SLOT(ChatClicked()));
	varVLC->le_chat->setEnabled(false);
	varVLC->pb_chat->setEnabled(false);
	varVLC->tx_chat->setReadOnly(true);
	return p;
}
QWidget *LayoutVLC::CreateTabMetrics()
{
	QWidget *p = new QWidget(mw);
	QGridLayout *grid = new QGridLayout(p);
	//Errors
	QGroupBox *gBoxErrors = new QGroupBox(tr("Errors"));
	gridErrors = new QGridLayout;
	gBoxErrors->setLayout(gridErrors);
        gridErrors->addWidget(new QLabel(tr(" ")), 0, 0);
	gridErrors->addWidget(new QLabel(tr(" ")), 0, 1, 1, 3);
	//gridErrors->addWidget(new QLabel(tr("BER: ")), 0, 0);
	//gridErrors->addWidget(new QLabel(tr("0")), 0, 1, 1, 3);
	gridErrors->addWidget(new QLabel(tr("PER: ")), 1, 0);
	gridErrors->addWidget(new QLabel(tr("0")), 1, 1, 1, 3);
	gridErrors->addWidget(new QLabel(tr("Lost packets: ")), 2, 0);
	gridErrors->addWidget(new QLabel(tr("0")), 2, 1);
	gridErrors->addWidget(new QLabel(tr("PHR CRC: ")), 3, 0);
	gridErrors->addWidget(new QLabel(tr("0")), 3, 1);
	gridErrors->addWidget(new QLabel(tr(" / ")), 3, 2);
	gridErrors->addWidget(new QLabel(tr("0")), 3, 3);
	gridErrors->addWidget(new QLabel(tr("PSDU CRC: ")), 4, 0);
	gridErrors->addWidget(new QLabel(tr("0")), 4, 1);
	gridErrors->addWidget(new QLabel(tr(" / ")), 4, 2);
	gridErrors->addWidget(new QLabel(tr("0")), 4, 3);
	//gridErrors->setRowStretch(1,1);
	//gridErrors->setColumnStretch(1,1);
	//Synching
	QGroupBox *gBoxSynch = new QGroupBox(tr("Link info"));
	gridLink = new QGridLayout;
	gBoxSynch->setLayout(gridLink);
	gridLink->addWidget(new QLabel(tr("Synching: ")), 0, 0);
	QLabel *la_synch = new QLabel(QString("<b><font color=red>Fail</font></b>"));
	la_synch->setTextFormat(Qt::AutoText);
	gridLink->addWidget(la_synch, 0, 1);
	gridLink->addWidget(new QLabel(tr("Speed: ")), 1, 0);
	gridLink->addWidget(new QLabel(tr("0")), 1, 1);
	gridLink->addWidget(new QLabel(tr("Kb/s")), 1, 2);
	gridLink->addWidget(new QLabel(tr("Total packets: ")), 2, 0);
	gridLink->addWidget(new QLabel(tr("0")), 2, 1);
	//Meas
	QGroupBox *gBoxMeas = new QGroupBox(tr("Measurements"));
	gridMeas = new QGridLayout;
	gBoxMeas->setLayout(gridMeas);
	gridMeas->addWidget(new QLabel(tr("SNR: ")), 0, 0);
	gridMeas->addWidget(new QLabel(tr("0")), 0, 1);
	gridMeas->addWidget(new QLabel(tr(" dB")), 0, 2);
	gridMeas->addWidget(new QLabel(tr("Power: ")), 1, 0);
	gridMeas->addWidget(new QLabel(tr("0")), 1, 1);
	gridMeas->addWidget(new QLabel(tr(" dB")), 1, 2);
	//gridMeas->setColumnStretch(1,1);
	//
	grid->addWidget(gBoxErrors, 0, 1);
	grid->addWidget(gBoxSynch, 0, 0);
	grid->addWidget(gBoxMeas, 1, 0);
	grid->setRowStretch(2, 1);
	//grid->setColumnStretch(2, 1);
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
	varVLC->sp_frame_size[0]->setRange(7,1023);
	varVLC->sp_frame_size[0]->setSingleStep(100);
	varVLC->sp_frame_size[0]->setHidden(false);
	QObject::connect(varVLC->sp_frame_size[0], SIGNAL(valueChanged(int)), this, SLOT(TrackChanges()));
	
	varVLC->sp_frame_size[1] = new QSpinBox(p);
	varVLC->sp_frame_size[1]->setRange(7,65535);
	varVLC->sp_frame_size[1]->setSingleStep(500);
	varVLC->sp_frame_size[1]->setHidden(true);
	QObject::connect(varVLC->sp_frame_size[1], SIGNAL(valueChanged(int)), this, SLOT(TrackChanges()));
		
	varVLC->ch_media = new QCheckBox(tr("Enable Media"), p);
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
	s->setValue("VLC/media", varVLC->ch_media->checkState());
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
	varVLC->ch_media->setCheckState((Qt::CheckState)s->value("VLC/media", 0).toInt());
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
	int checked1 = 0,checked2 = 0;
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
			varVLC->sp_frame_size[0]->setHidden(false);
			varVLC->sp_frame_size[1]->setHidden(true);
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
			varVLC->sp_frame_size[0]->setHidden(true);
			varVLC->sp_frame_size[1]->setHidden(false);
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
void LayoutVLC::StateLayout(MainWindow::StatesLayout s)
{
	if (s == MainWindow::STOPPING)
		Stop();
}
void LayoutVLC::ChatClicked()
{
	int size = varVLC->le_chat->text().size();
	if (tx && size)
	{
		unsigned char *t = new unsigned char[size+1];
		memcpy(t, (unsigned char *)varVLC->le_chat->text().toStdString().c_str(), sizeof(unsigned char)*(size+1));
		t[size] = '\0';
		tx->SendPacket(t, size+1);
		varVLC->tx_chat->setPlainText(varVLC->tx_chat->toPlainText().append(varVLC->le_chat->text().append('\n')));
		varVLC->le_chat->setText('\0');
		QTextCursor c = varVLC->tx_chat->textCursor();
		c.movePosition(QTextCursor::End);
		varVLC->tx_chat->setTextCursor(c);
	}
}
void LayoutVLC::ChatAppend(QString &s)
{
	emit ChatSigText(s);
}
void LayoutVLC::ChatText(QString &s)
{
	varVLC->tx_chat->setPlainText(varVLC->tx_chat->toPlainText().append(s));
	QTextCursor c = varVLC->tx_chat->textCursor();
	c.movePosition(QTextCursor::End);
	varVLC->tx_chat->setTextCursor(c);
}
//size is the size of in. out must have size+CRC_LENGTH space
unsigned short LayoutVLC::GenerateCRC(int *in, int *out, int size)
{
	int crc = 0xffff, *dt = in;
	for (int i = 0; i < size; i += 8, dt += 8)
	{
		unsigned char d = *dt | *(dt+1)<<1 | *(dt+2)<<2 | *(dt+3)<<3 | *(dt+4)<<4 | *(dt+5)<<5 | *(dt+6)<<6 | *(dt+7)<<7;
		unsigned crc_new = ((unsigned char)(crc >> 8) | (crc << 8)) ^ d;
		crc_new ^= (unsigned char)(crc_new & 0xff) >> 4;
		crc_new ^= crc_new << 12;
		crc_new ^= (crc_new & 0xff) << 5;
		crc = crc_new;
	}
	if (out)
	{
		if (in != out)
			memcpy(out, in, sizeof(int)*size);
		out += size;
		for (int i = 0; i < CRC_LENGTH; i++)
			*out++ = (crc>>i)&0x1;
	}
	return crc;
}
//CRC is appended at the end of in. Size is the size of in (including CRC)
bool LayoutVLC::CheckCRC(int *in, int size)
{
	unsigned short crc = GenerateCRC(in, NULL, size-CRC_LENGTH), excrc = 0x0;
	int *dt = in+size-CRC_LENGTH;
	for (int i = 0; i < CRC_LENGTH; i++)
		excrc |= dt[i]<<i;
	return (excrc == crc);
}
void LayoutVLC::EmitChangeMetric(QLabel *la, QString s)
{
	emit ChangeMetric(la, s);
}
void LayoutVLC::ChangedMetric(QLabel *la, QString s)
{
	la->setText(s);
}
void LayoutVLC::UpdateSpeed()
{
	mtx.lock();
	((QLabel *)gridLink->itemAtPosition(1,1)->widget())->setText(QString::number((double)bits/(++secs*1024),'g',4));
	if (syncs)
		((QLabel *)gridLink->itemAtPosition(0, 1)->widget())->setText(QString("<b><font color=green>Ok!</font></b>"));
	else
		((QLabel *)gridLink->itemAtPosition(0, 1)->widget())->setText(QString("<b><font color=red>Fail</font></b>"));
	syncs = 0;
	mtx.unlock();
}
void LayoutVLC::SendReport()
{
	char buf[512];
	sprintf(buf, "php -q send_sms.php \"BER: %f, PER: %f, Lost: %d, Total: %d, PHR CRC: %d/%d, PSDU CRC: %d/%d, Rate: %.2f\"", 
		((QLabel *)gridErrors->itemAtPosition(0, 1)->widget())->text().toFloat(),
		((QLabel *)gridErrors->itemAtPosition(1, 1)->widget())->text().toFloat(),
		((QLabel *)gridErrors->itemAtPosition(2, 1)->widget())->text().toInt(),
		((QLabel *)gridLink->itemAtPosition(2, 1)->widget())->text().toInt(),
		((QLabel *)gridErrors->itemAtPosition(3, 1)->widget())->text().toInt(),
		((QLabel *)gridErrors->itemAtPosition(3, 3)->widget())->text().toInt(),
		((QLabel *)gridErrors->itemAtPosition(4, 1)->widget())->text().toInt(),
		((QLabel *)gridErrors->itemAtPosition(4, 3)->widget())->text().toInt(),
		((QLabel *)gridLink->itemAtPosition(1, 1)->widget())->text().toFloat()
		);
	system(buf);
}
int LayoutVLC::GetModulatedResources(PHYType phy_type, Modulation mod, int rate, int raw_length)
{
	int length = 0, rs_bits = raw_length, cc_bits = 0;
	int rs_in = 0, cc_in = 0, rs_out = 0, cc_out = 0, gf = 0;
	if (phy_type == PHY_I)
	{
		gf = 4;
		if ((rate < 4 && mod == OOK) || (rate < 3 && mod == VPPM))
		{
			rs_out = 15;
			if (mod == OOK)
			{
				if (rate == 0)
					rs_in = 7;
				else 
					rs_in = 11;
			}
			else if (mod == VPPM)
			{
				if (rate == 0)
					rs_in = 2;
				else if (rate == 1)
					rs_in = 4;
				else if (rate == 2)
					rs_in = 7;
			}
		}
		if (mod == OOK)
		{
			if (rate == 0)
			{
				cc_in = 1;
				cc_out = 4;
			}
			else if (rate == 1)
			{
				cc_in = 1;
				cc_out = 3;
			}
			else if (rate == 2)
			{
				cc_in = 2;
				cc_out = 3;
			}
		}
	}
	else if (phy_type == PHY_II)
	{
		gf = 8;
		if (rate == 0 || rate == 2 || rate == 4 || rate == 6)
		{
			rs_in = 32;
			rs_out = 64;
		}
		else if (rate == 1 || rate == 3 || rate == 5 || rate == 7)
		{
			rs_in = 128;
			rs_out = 160;
		}
	}
	if (rs_in)
	{
		int GF_words = ceil(((double)raw_length/gf));
		if (!(GF_words%rs_in))
			rs_bits = (GF_words/rs_in)*rs_out*gf;
		else
			rs_bits = ((GF_words/rs_in)*rs_out+ GF_words%rs_in + (rs_out-rs_in))*gf;		
	}
	if (cc_in)
		cc_bits= ((rs_bits + 6)*cc_out)/cc_in;
	else
		cc_bits = rs_bits;
	if (phy_type == PHY_I)
	{
		if (mod == OOK)
			length = cc_bits*2;
		else if (mod == VPPM)
			length = cc_bits*6/4;
	}
	else if (phy_type == PHY_II)
	{
		if (mod == OOK)
			length = cc_bits*10/8;
		else if (mod == VPPM)
			length = cc_bits*6/4;
	}

	return length;
}
