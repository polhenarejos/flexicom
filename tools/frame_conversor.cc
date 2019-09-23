/* 
 * This file is part of the FlexiCom distribution (https://github.com/polhenarejos/flexicom).
 * Copyright (c) 2012-2020 Pol Henarejos, at Centre Tecnologic de Telecomunicacions de Catalunya (CTTC).
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/* $Id$ */
/* $Format:%ci$ ($Format:%h$) by $Format:%an%$ */

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
