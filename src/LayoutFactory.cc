// $Id$

#include "LayoutFactory.h"
#include "MainWindow.h"
#include "modules/SHM.cc"

LayoutFactory::LayoutFactory(MainWindow *_mw, int _radioID) :
	mw(_mw), radioID(_radioID)
{
}
int LayoutFactory::Sources()
{
	if (rx)
		return mw->panel->sp_devs->value();
	return 0;
}
int LayoutFactory::Sinks()
{
	if (!rx)
		return mw->panel->sp_devs->value();
	return 0;
}
boost::shared_ptr<gr_basic_block> LayoutFactory::Source(int s)
{
	if (dev == 1)
		return gnuradio::detail::sptr_magic::fetch_initial_sptr(usrp_source.get());
	else if (dev == 2)
		return gnuradio::detail::sptr_magic::fetch_initial_sptr(file_source[s].get());
	else if (dev == 3)
		return gnuradio::detail::sptr_magic::fetch_initial_sptr(shm_source[s].get());
	else if (dev == 4)
		return gnuradio::detail::sptr_magic::fetch_initial_sptr(tcp_source[s].get());
	else if (dev == 5)
		return gnuradio::detail::sptr_magic::fetch_initial_sptr(udp_source[s].get());
	return gnuradio::detail::sptr_magic::fetch_initial_sptr(null_source[s].get());
}
boost::shared_ptr<gr_basic_block> LayoutFactory::Sink(int s)
{
	if (dev == 1)
		return gnuradio::detail::sptr_magic::fetch_initial_sptr(usrp_sink.get());
	else if (dev == 2)
		return gnuradio::detail::sptr_magic::fetch_initial_sptr(file_sink[s].get());
	else if (dev == 3)
		return gnuradio::detail::sptr_magic::fetch_initial_sptr(shm_sink[s].get());
	else if (dev == 4)
		return gnuradio::detail::sptr_magic::fetch_initial_sptr(tcp_sink[s].get());
	else if (dev == 5)
		return gnuradio::detail::sptr_magic::fetch_initial_sptr(udp_sink[s].get());
	else if (dev == 6)
		return gnuradio::detail::sptr_magic::fetch_initial_sptr(matlab_sink[s].get());
	return gnuradio::detail::sptr_magic::fetch_initial_sptr(null_sink[s].get());
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
	QString addr = QString("addr0=%1").arg(mw->panel->le_ip[0]->text().remove(' '));
	for (int i = 1; i < devs; i++)
		addr.append(",addr%1=%2").arg(i).arg(mw->panel->le_ip[i]->text().remove(' '));
	if ((rx = mw->panel->rb_chain[RB_RX]->isChecked()))
	{
		if (dev == 1)
		{
			usrp_source = uhd_make_usrp_source(addr.toStdString(), uhd::stream_args_t("fc32","sc16"));
			usrp_source->set_gain(mw->panel->sp_gain->value());
			usrp_source->set_center_freq(mw->panel->sp_freq->value()*1e6);
			usrp_source->set_samp_rate(mw->panel->sp_sps->value()*1e3);
		}
		else
		{
			for (int j = 0; j < devs; j++)
			{
				if (dev == 0)
					null_source[j] = gr_make_null_source(sizeof(gr_complex));
				else if (dev == 2)
					file_source[j] = gr_make_file_source(sizeof(gr_complex), mw->panel->le_name[j]->text().toAscii().data(), false);
				else if (dev == 3)
					shm_source[j] = SHMSource<gr_complex>::Create(mw->panel->le_name[j]->text().toAscii().data());
				else if (dev == 4)
					tcp_source[j] = TcpSource::Create(sizeof(gr_complex), mw->panel->le_ip[0]->text().toAscii().data(), mw->panel->sp_port[j]->value());
				else if (dev == 5)
					udp_source[j] = gr_make_udp_source(sizeof(gr_complex), mw->panel->le_ip[0]->text().toAscii().data(), mw->panel->sp_port[j]->value());
			}
		}
	}
	else
	{
		if (dev == 1)
		{
			usrp_sink = uhd_make_usrp_sink(addr.toStdString(), uhd::stream_args_t("fc32","sc8"));
			usrp_sink->set_gain(mw->panel->sp_gain->value());
			usrp_sink->set_center_freq(mw->panel->sp_freq->value()*1e6);
			usrp_sink->set_samp_rate(mw->panel->sp_sps->value()*1e3);
		}
		else
		{
			for (int j = 0; j < devs; j++)
			{
				if (dev == 0)
					null_sink[j] = gr_make_null_sink(sizeof(gr_complex));
				else if (dev == 2)
					file_sink[j] = gr_make_file_sink(sizeof(gr_complex), mw->panel->le_name[j]->text().toAscii().data());
				else if (dev == 3)
					shm_sink[j] = SHMSink<gr_complex>::Create(mw->panel->le_name[j]->text().toAscii().data());
				else if (dev == 4)
					tcp_sink[j] = TcpSink::Create(sizeof(gr_complex), mw->panel->le_ip[0]->text().toAscii().data(), mw->panel->sp_port[j]->value());
				else if (dev == 5)
					udp_sink[j] = gr_make_udp_sink(sizeof(gr_complex), mw->panel->le_ip[0]->text().toAscii().data(), mw->panel->sp_port[j]->value());
				else if (dev == 6)
					matlab_sink[j] = bbMatlab::Create(mw->panel->le_name[j]->text().toAscii().data(), sizeof(gr_complex));
			}
		}
	}
}
