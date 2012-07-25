// $Id$
#include "vlc_crc.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

vlc_crc::vlc_crc() 
//:length_data(_length)
{
	crc_length = 16;
	poly[0]=16; poly[1]= 12;
	poly[2]=5; poly[3]= 0;
				 
}

vlc_crc::~vlc_crc ()
{
}

void vlc_crc::generate_crc(int *data, int *out, int size)
{
	int tmp[4];
	unsigned int i;
		//tab_len = length_data + crc_length;
	int *tab_ops = new int[size];
	int *shift_reg = new int[size];
	int tb;
	
	memset(shift_reg, 0, sizeof(int)*size);
	memset(tab_ops,0, sizeof(int)*size);
	memcpy(shift_reg,data, sizeof(int)*(size-crc_length));
	
	for (i=0; i<sizeof(poly)/sizeof(int);i++)
		tmp[i]= crc_length- poly[i];
	for (i=1; i<sizeof(poly)/sizeof(int); i++)
		tab_ops[tmp[i]-1]=1;

	for (i = 0; i < (size-crc_length); i++)
	{
		tb = shift_reg[0];
		for (int j = 0; j < size-1; j++)
		{
			if (tab_ops[j] == 1)
				shift_reg[j] = shift_reg[j+1] ^ tb;
			else
				shift_reg[j] = shift_reg[j+1];
		}
	}
	memcpy(out,data,sizeof(int)*(size-crc_length));
	memcpy(&out[size-crc_length],shift_reg, sizeof(int)*crc_length);
	delete [] tab_ops;
	delete [] shift_reg;
	return;
}

bool vlc_crc::check_crc( int *data, int *out, int size)
{
	int i,ord=poly[0];
	int *error=new int[size];
    bool ok=true;
	generate_crc(data, error,size);
	for (i=0; i < ord; i++)
	{
		if (error[size-ord+i]!=0)
		{
			ok=false;
			break;
		}
	}
	if (out)
		memcpy(out,&data[size-2*crc_length],sizeof(int)*crc_length);
	//we copy in out the crc
	delete [] error;
	return ok;
}
