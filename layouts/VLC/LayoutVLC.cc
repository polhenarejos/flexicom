#include "LayoutVLC.h"

const char *LayoutVLC::name = "VLC";

LayoutVLC::LayoutVLC(MainWindow *_mw, int _radioID) :
	LayoutFactory()
{
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
}
void LayoutVLC::Stop()
{
}
