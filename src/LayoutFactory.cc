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

#include "LayoutFactory.h"
#include "MainWindow.h"
#include "modules/SHM.cc"
#include "modules/Oversampler.cc"
#include "bbMatlab.h"
#include "Tcp.h"
#include <gnuradio/sptr_magic.h>
#include <gnuradio/uhd/usrp_source.h>
#include <gnuradio/uhd/usrp_sink.h>
#include <gnuradio/blocks/file_source.h>
#include <gnuradio/blocks/file_sink.h>
#include <gnuradio/blocks/udp_sink.h>
#include <gnuradio/blocks/udp_source.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/null_source.h>
#include <gnuradio/blocks/multiply_const.h>
#include <gnuradio/io_signature.h>

LayoutFactory::LayoutFactory(MainWindow *_mw, int _radioID) :
	mw(_mw), radioID(_radioID)
{
}
GeneralSource::sptr LayoutFactory::Source()
{
	return gen_source;
}
GeneralSink::sptr LayoutFactory::Sink()
{
	return gen_sink;
}
void LayoutFactory::Setup()
{
	devs = mw->panel->sp_devs->value();
	for (int i = 0; i < sizeof(mw->panel->rb_dev)/sizeof(QRadioButton *); i++)
	{
		if (mw->panel->rb_dev[i]->isChecked())
		{
			dev = i;
			break;
		}
	}
	if ((rx = mw->panel->rb_chain[RB_RX]->isChecked()))
		gen_source = GeneralSource::Create(dev, devs, mw->panel);
	else
		gen_sink = GeneralSink::Create(dev, devs, mw->panel);
}
GeneralSink::GeneralSink(int dev, int devs, Panel *panel) :
	gr::hier_block2("GeneralSink", gr::io_signature::make(1, devs, sizeof(gr_complex)), gr::io_signature::make(0, 0, 0))
{
	auto end = self();
	if (panel->ch_ov->checkState() == Qt::Checked)
	{
		Oversampler<gr_complex>::sptr ovb = Oversampler<gr_complex>::Create(panel->sp_ov->value());
		for (int j = 0; j < devs; j++)
			connect(self(), j, ovb, j);
		end = ovb->to_basic_block();
	}
	if (dev != 1)
	{
		for (int j = 0; j < devs; j++)
		{
			if (dev == 0)
				connect(end, j, gr::blocks::null_sink::make(sizeof(gr_complex)), 0);
			else if (dev == 2)
				connect(end, j, gr::blocks::file_sink::make(sizeof(gr_complex), panel->le_name[j]->text().toStdString().c_str()), 0);
			else if (dev == 3)
				connect(end, j, SHMSink<gr_complex>::Create(panel->le_name[j]->text().toStdString().c_str()), 0);
			else if (dev == 4)
				connect(end, j, TcpSink::Create(sizeof(gr_complex), panel->le_ip[0]->text().toStdString().c_str(), panel->sp_port[j]->value()), 0);
			else if (dev == 5)
				connect(end, j, gr::blocks::udp_sink::make(sizeof(gr_complex), panel->le_ip[0]->text().toStdString().c_str(), panel->sp_port[j]->value()), 0);
			else if (dev == 6)
				connect(end, j, bbMatlab::Create(panel->le_name[j]->text().toStdString().c_str(), sizeof(gr_complex)), 0);
		}
	}
	else
	{
		QString addr = QString("addr0=%1").arg(panel->le_ip[0]->text().remove(' '));
		for (int i = 1; i < devs; i++)
			addr.append(",addr%1=%2").arg(i).arg(panel->le_ip[i]->text().remove(' '));
		gr::uhd::usrp_sink::sptr usrp_sink = gr::uhd::usrp_sink::make(addr.toStdString(), uhd::stream_args_t("fc32","sc8"));
		usrp_sink->set_gain(panel->sp_gain->value());
		usrp_sink->set_center_freq(panel->sp_freq->value()*1e6);
		usrp_sink->set_samp_rate(panel->sp_sps->value()*1e3);
		connect(end, 0, usrp_sink, 0);
	}
}
GeneralSink::sptr GeneralSink::Create(int dev, int devs, Panel *panel)
{
	return gnuradio::get_initial_sptr(new GeneralSink(dev, devs, panel));
}
GeneralSource::GeneralSource(int dev, int devs, Panel *panel) :
	gr::hier_block2("GeneralSource", gr::io_signature::make(0, 0, 0), gr::io_signature::make(1, devs, sizeof(gr_complex)))
{
	auto end = self();
	if (panel->ch_flip->checkState() == Qt::Checked)
	{
		GeneralFlip::sptr fl = GeneralFlip::Create(devs);
		for (int j = 0; j < devs; j++)
			connect(fl, j, self(), j);
		end = fl->self();
	}
	if (dev != 1)
	{
		for (int j = 0; j < devs; j++)
		{
			if (dev == 0)
				connect(gr::blocks::null_source::make(sizeof(gr_complex)), 0, end, j);
			else if (dev == 2)
				connect(gr::blocks::file_source::make(sizeof(gr_complex), panel->le_name[j]->text().toStdString().c_str(), false), 0, end, j);
			else if (dev == 3)
				connect(SHMSource<gr_complex>::Create(panel->le_name[j]->text().toStdString().c_str()), 0, end, j);
			else if (dev == 4)
				connect(TcpSource::Create(sizeof(gr_complex), panel->le_ip[0]->text().toStdString().c_str(), panel->sp_port[j]->value()), 0, end, j);
			else if (dev == 5)
				connect(gr::blocks::udp_source::make(sizeof(gr_complex), panel->le_ip[0]->text().toStdString().c_str(), panel->sp_port[j]->value()), 0, end, j);
		}
	}
	else
	{
		QString addr = QString("addr0=%1").arg(panel->le_ip[0]->text().remove(' '));
		for (int i = 1; i < devs; i++)
			addr.append(",addr%1=%2").arg(i).arg(panel->le_ip[i]->text().remove(' '));
		gr::uhd::usrp_source::sptr usrp_source = gr::uhd::usrp_source::make(addr.toStdString(), uhd::stream_args_t("fc32","sc16"));
		usrp_source->set_gain(panel->sp_gain->value());
		usrp_source->set_center_freq(panel->sp_freq->value()*1e6);
		usrp_source->set_samp_rate(panel->sp_sps->value()*1e3);
		connect(usrp_source, 0, end, 0);
	}
}
GeneralSource::sptr GeneralSource::Create(int dev, int devs, Panel *panel)
{
	return gnuradio::get_initial_sptr(new GeneralSource(dev, devs, panel));
}
GeneralFlip::GeneralFlip(int devs) :
	gr::hier_block2("GeneralFlip", gr::io_signature::make(devs, devs, sizeof(gr_complex)), gr::io_signature::make(devs, devs, sizeof(gr_complex)))
{
	for (int j = 0; j < devs; j++)
	{
		gr::blocks::multiply_const_cc::sptr mc = gr::blocks::multiply_const_cc::make(gr_complex(-1,0));
		connect(self(), j, mc, 0);
		connect(mc, 0, self(), j);
	}
}
GeneralFlip::sptr GeneralFlip::Create(int devs)
{
	return gnuradio::get_initial_sptr(new GeneralFlip(devs));
}
