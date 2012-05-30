#ifndef _INC_LAYOUTVLC_H_
#define _INC_LAYOUTVLC_H_

#include "LayoutFactory.h"

class LayoutVLC : public LayoutFactory
{
	private:
		static const char *name;
	public:
		LayoutVLC();
		const char *Name();
		static LayoutFactory::sptr Create();
		void Run();
		void Stop();
};

#endif //_INC_LAYOUTVLC_H_
