#ifndef _INC_LAYOUT80211B_H_
#define _INC_LAYOUT80211B_H_

#include "LayoutFactory.h"
#include <gr_top_block.h>

class Layout80211b : public LayoutFactory
{
	private:
		static const char *name;
		gr_top_block_sptr grTop;
		
	public:
		Layout80211b();
		const char *Name();
		static LayoutFactory::sptr Create();
		void Run();
		void Stop();
};

#endif //_INC_LAYOUT80211B_H_
