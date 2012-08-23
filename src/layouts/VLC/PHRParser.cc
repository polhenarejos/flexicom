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

	for (int n = 0; n < no;)
	{
		int mv = 1;
		if (ic == 0)
			printf("---------------------\nPHR HEADER\n| Burst_mode: %d\n", *iptr);
		else if (ic == 1)
		{
			if (n+3 < no)
			{
				printf("| Channel_number: %d\n", LayoutVLC::bi2dec((int *)iptr, 3));
				mv = 3;
			}
			else
				break;
		}
		else if (ic == 4)
		{
			if (n+6 < no)
			{
				printf("| MCSID: %d\n", LayoutVLC::bi2dec((int *)iptr, 6));
				mv = 6;
			}
			else
				break;
		}
		else if (ic == 10)
		{
			if (n+16 < no)
			{
				printf("| PSDU_length: %d bytes\n", LayoutVLC::bi2dec((int *)iptr, 16));
				mv = 16;
			}
			else
				break;
		}
		else if (ic == 26)
			printf("| Dimmed_OOK_extension: %d\n", *iptr);
		csmd += mv;
		iptr += mv;
		ic = (ic+mv)%32;
		n += mv;
	}
	consume_each(csmd);
	return 0;
}
