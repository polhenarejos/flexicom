// $Id: frame_conversor.cc 433 2010-06-22 07:40:45Z phenarejos $

#include <iostream>
#include <stdio.h>
#include <complex>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		std::cout<<"USE "<<argv[0]<<" FILE [c]"<<std::endl;
		exit(-1);
	}
	char buf[1024], *real, *imag;
	//int i = 0;
	sprintf(buf, "%s.dat", argv[1]);
	bool force_complex = (argc > 2 && *argv[2] == 'c');
	FILE *fp = fopen(argv[1], "r"), *fw = fopen(buf, "wb");
	while (fgets(buf, 1024, fp))
	{
		//std::cout<<"Line: "<<++i<<std::endl;
		buf[strlen(buf)-1]=0;
		real = buf;
		while (*real==' ')real++;
		if ((imag = strchr(real,' ')))
		{
			*imag++=0;
			while (*imag==' ')imag++;
			std::complex<float> comp(atof(real),atof(imag));
			fwrite(&comp, sizeof(std::complex<float>), 1, fw);
		}
		else
		{
			if (force_complex)
			{
				std::complex<float> comp(atof(real), 0);
				fwrite(&comp, sizeof(std::complex<float>), 1, fw);
			}
			else
			{
				float comp = atof(real);
				fwrite(&comp, sizeof(float), 1, fw);
			}
		}
	}
	fclose(fw);
	fclose(fp);
	return 0;
}
