#include <vlc_crc.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

vlc_crc::vlc_crc(int _length) :
				 length_data(_length)
{
	crc_length = 16;
	poly = new int[4];
	poly[0]=16; poly[1]= 12;
	poly[2]=5; poly[4]= 0;
				 
}

vlc_crc::~vlc_crc ()
{
  if (poly)
  {
	  delete poly;
	  poly =0;
  }
}

void vlc_crc::generate_crc(int *data, int *out)
{
	int *tmp = new int [4];
	int i,tab_len = length_data + crc_length;
	//tmp = new int[tab_len];
	int *tab_ops = new int[tab_len];
	int *shift_reg = new int[tab_len];
	int tb;
	
	memset(shift_reg,1, sizeof(int)*tab_len);
	memset(tab_ops,0, sizeof(int)*tab_len);
	memcpy(shift_reg,data, sizeof(int)*length_data);
	
	for (i=0; i<sizeof(poly)/sizeof(int);i++)
		tmp[i]= crc_length- poly[i];
	for (i=1; i<sizeof(poly)/sizeof(int); i++)
		tab_ops[tmp[i]-1]=1;
	for (i = 0; i < length_data; i++)
	{
		tb = shift_reg[0];
		for (int j = 0; j < tab_len-1; j++)
		{
			if (tab_ops[j] == 1)
				shift_reg[j] = shift_reg[j+1] ^ tb;
			else
				shift_reg[j] = shift_reg[j+1];
		}
	}
	memcpy(out,data,sizeof(int)*length_data);
	memcpy(&out[length_data],shift_reg, sizeof(int)*crc_length);
	return;
}

void vlc_crc::check_crc( int *data, int *out, bool *ok)
{
	int i,ord=poly[0];
	int tab_len = length_data + crc_length;
	int *error=new int[tab_len];
    *ok=true;
	generate_crc(data, error);
	for (i=0; i < ord; i++)
	{
		if (error[tab_len-ord+i]!=0)
		{
			ok=false;
			break;
		}
	}
	memcpy(out,&data[length_data],sizeof(int)*crc_length);
	//we copy in out the crc
	
}