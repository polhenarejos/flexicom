// $Id$

#include "PHRParser.h"
#include "LayoutVLC.h"
#include <gr_io_signature.h>

PHRParser::PHRParser() :
	gr_block("PHRParser", gr_make_io_signature(1, 1, sizeof(int)), gr_make_io_signature(0, 0, 0)),
	ic(0)
{
}
PHRParser::sptr PHRParser::Create()
{
	return sptr(new PHRParser());
}
#include <QMutex>
extern QMutex mtx;
int PHRParser::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	const int *iptr = (const int *)_i[0];
	int csmd = 0;

	for (int n = 0; n < no; n++)
	{
		int mv = 0;
		if (ic == 0)
		{
			printf("---------------------\nEXTRACTING INFORMATION FROM PHR HEADER\n");
			printf("| Burst_mode: %d\n", *iptr);
			mv = 1;
		}
		else if (ic == 1 && n+3 < no)
		{
			printf("| Channel_number: %d\n", LayoutVLC::bi2dec((int *)iptr, 3));
			mv = 3;
		}
		else if (ic == 4 && n+6 < no)
		{
			printf("| MCSID: %d\n", LayoutVLC::bi2dec((int *)iptr, 6));
			mv = 6;
		}
		else if (ic == 10 && n+16 < no)
		{
			printf("| PSDU_length: %d bytes\n", LayoutVLC::bi2dec((int *)iptr, 16));
			mv = 16;
		}
		else if (ic == 26)
		{
			printf("| Dimmed_OOK_extension: %d\n", *iptr);
			mv = 1;
		}
		csmd += mv;
		iptr += mv;
		ic = (ic+1)%32;
	}
	consume_each(csmd);
	return 0;
}
