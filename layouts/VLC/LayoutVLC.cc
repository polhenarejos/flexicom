#include "LayoutVLC.h"

const char *LayoutVLC::name = "VLC";

LayoutVLC::LayoutVLC()
	: LayoutFactory()
{
}
const char *LayoutVLC::Name()
{
	return name;
}
LayoutFactory::sptr LayoutVLC::Create()
{
	return LayoutFactory::sptr(new LayoutVLC());
}
void LayoutVLC::Run()
{
}
void LayoutVLC::Stop()
{
}
