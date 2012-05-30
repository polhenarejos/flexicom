#include "Layout80211b.h"
#include <gr_uhd_usrp_source.h>
#include "Rx80211b.h"
#include <iostream>

const char *Layout80211b::name = "802.11b";

Layout80211b::Layout80211b()
	: LayoutFactory()
{
}
const char *Layout80211b::Name()
{
	return name;
}
LayoutFactory::sptr Layout80211b::Create()
{
	return LayoutFactory::sptr(new Layout80211b());
}
void Layout80211b::Run()
{
	grTop = gr_make_top_block(std::string(name));
	boost::shared_ptr<uhd_usrp_source> usrp_source = uhd_make_usrp_source(std::string("addr=192.168.10.2"), uhd::stream_args_t("fc32","sc8"));
	usrp_source->set_samp_rate(10e6);
    usrp_source->set_center_freq(2462e6);
    usrp_source->set_gain(50);
    Rx80211b::sptr rx = Rx80211b::Create();
    grTop->connect(usrp_source, 0, rx, 0);
    grTop->start();
}
void Layout80211b::Stop()
{
	grTop->stop();
	grTop->wait();
}
