// $Id$

#ifndef _INC_LAYOUTCOVLC_H_
#define _INC_LAYOUTCOVLC_H_

#include "LayoutFactory.h"
#include "MainWindow.h"
#include <gr_top_block.h>

class LayoutCoVLC : public QObject , public LayoutFactory
{
	Q_OBJECT
	
	private:
		static const char *name;
		gr_top_block_sptr grTop;
		
	public:
		LayoutCoVLC(MainWindow *, int);
		const char *Name();
		static LayoutFactory::sptr Create(MainWindow *, int);
		void Run();
		void Stop();
};

#endif //_INC_LAYOUTCOVLC_H_
