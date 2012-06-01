#ifndef _INC_LAYOUTVLC_H_
#define _INC_LAYOUTVLC_H_

#include "LayoutFactory.h"

class LayoutVLC : public LayoutFactory
{
	private:
		static const char *name;
	public:
		LayoutVLC(MainWindow *, int);
		static const char *Name();
		static LayoutFactory::sptr Create(MainWindow *, int);
		void Run();
		void Stop();
};

#endif //_INC_LAYOUTVLC_H_
