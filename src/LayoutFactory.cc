// $Id$

#include "LayoutFactory.h"
#include "MainWindow.h"
#include "modules/SHM.cc"
#include "modules/Oversampler.cc"
#include "bbMatlab.h"
#include "Tcp.h"
#include <gr_sptr_magic.h>
#include <gr_uhd_usrp_source.h>
#include <gr_uhd_usrp_sink.h>
#include <gr_file_source.h>
#include <gr_file_sink.h>
#include <gr_udp_sink.h>
#include <gr_udp_source.h>
#include <gr_null_sink.h>
#include <gr_null_source.h>
#include <gr_multiply_const_cc.h>
#include <gr_io_signature.h>

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
	gr_hier_block2("GeneralSink", gr_make_io_signature(1, devs, sizeof(gr_complex)), gr_make_io_signature(0, 0, 0))
{
	boost::shared_ptr<gr_basic_block> end = self();
	if (panel->ch_ov->checkState() == Qt::Checked)
	{
		Oversampler<gr_complex>::sptr ovb = Oversampler<gr_complex>::Create(panel->sp_ov->value());
		for (int j = 0; j < devs; j++)
			connect(self(), j, ovb, j);
		end = gnuradio::detail::sptr_magic::fetch_initial_sptr(ovb.get());
	}
	if (dev != 1)
	{
		for (int j = 0; j < devs; j++)
		{
			if (dev == 0)
				connect(end, j, gr_make_null_sink(sizeof(gr_complex)), 0);
			else if (dev == 2)
				connect(end, j, gr_make_file_sink(sizeof(gr_complex), panel->le_name[j]->text().toAscii().data()), 0);
			else if (dev == 3)
				connect(end, j, SHMSink<gr_complex>::Create(panel->le_name[j]->text().toAscii().data()), 0);
			else if (dev == 4)
				connect(end, j, TcpSink::Create(sizeof(gr_complex), panel->le_ip[0]->text().toAscii().data(), panel->sp_port[j]->value()), 0);
			else if (dev == 5)
				connect(end, j, gr_make_udp_sink(sizeof(gr_complex), panel->le_ip[0]->text().toAscii().data(), panel->sp_port[j]->value()), 0);
			else if (dev == 6)
				connect(end, j, bbMatlab::Create(panel->le_name[j]->text().toAscii().data(), sizeof(gr_complex)), 0);
		}
	}
	else
	{
		QString addr = QString("addr0=%1").arg(panel->le_ip[0]->text().remove(' '));
		for (int i = 1; i < devs; i++)
			addr.append(",addr%1=%2").arg(i).arg(panel->le_ip[i]->text().remove(' '));
		boost::shared_ptr<uhd_usrp_sink> usrp_sink = uhd_make_usrp_sink(addr.toStdString(), uhd::stream_args_t("fc32","sc8"));
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
	gr_hier_block2("GeneralSource", gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, devs, sizeof(gr_complex)))
{
	boost::shared_ptr<gr_basic_block> end = self();
	if (panel->ch_flip->checkState() == Qt::Checked)
	{
		GeneralFlip::sptr fl = GeneralFlip::Create(devs);
		for (int j = 0; j < devs; j++)
			connect(fl, j, self(), j);
		end = gnuradio::detail::sptr_magic::fetch_initial_sptr(fl.get());
	}
	if (dev != 1)
	{
		for (int j = 0; j < devs; j++)
		{
			if (dev == 0)
				connect(gr_make_null_source(sizeof(gr_complex)), 0, end, j);
			else if (dev == 2)
				connect(gr_make_file_source(sizeof(gr_complex), panel->le_name[j]->text().toAscii().data(), false), 0, end, j);
			else if (dev == 3)
				connect(SHMSource<gr_complex>::Create(panel->le_name[j]->text().toAscii().data()), 0, end, j);
			else if (dev == 4)
				connect(TcpSource::Create(sizeof(gr_complex), panel->le_ip[0]->text().toAscii().data(), panel->sp_port[j]->value()), 0, end, j);
			else if (dev == 5)
				connect(gr_make_udp_source(sizeof(gr_complex), panel->le_ip[0]->text().toAscii().data(), panel->sp_port[j]->value()), 0, end, j);
		}
	}
	else
	{
		QString addr = QString("addr0=%1").arg(panel->le_ip[0]->text().remove(' '));
		for (int i = 1; i < devs; i++)
			addr.append(",addr%1=%2").arg(i).arg(panel->le_ip[i]->text().remove(' '));
		boost::shared_ptr<uhd_usrp_source> usrp_source = uhd_make_usrp_source(addr.toStdString(), uhd::stream_args_t("fc32","sc16"));
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
	gr_hier_block2("GeneralFlip", gr_make_io_signature(devs, devs, sizeof(gr_complex)), gr_make_io_signature(devs, devs, sizeof(gr_complex)))
{
	for (int j = 0; j < devs; j++)
	{
		gr_multiply_const_cc_sptr mc = gr_make_multiply_const_cc(gr_complex(-1,0)); 
		connect(self(), j, mc, 0);
		connect(mc, 0, self(), j);
	}
}
GeneralFlip::sptr GeneralFlip::Create(int devs)
{
	return gnuradio::get_initial_sptr(new GeneralFlip(devs));
}
