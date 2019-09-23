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

#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <gnuradio/io_signature.h>
#include <complex>
#include "bbMatlab.h"
#ifdef _WIN
#include <io.h>
#endif

bbMatlab::~bbMatlab()
{
	fclose(fp);
}


bbMatlab::sptr bbMatlab::Create(std::string f, size_t s)
{
	return sptr(new bbMatlab(f, s));
}
bbMatlab::bbMatlab(std::string f, size_t _s) : 
	gr::sync_block("bbMatlab", gr::io_signature::make(1, gr::io_signature::IO_INFINITE, _s), gr::io_signature::make(0, 0, 0)),
	s(_s)
{
	int fd;
	if ((fd = open (f.c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0)
		throw std::runtime_error("Cannot open Matlab filename");
	if (!(fp = fdopen (fd, "w")))
	{
		close(fd);
		throw std::runtime_error("Cannot fdopen");
	}
	if (s == sizeof(gr_complex)) //64
		cplx = 1;
	else if (s == sizeof(float)) //32
		cplx = 0;
	else if (s == sizeof(gr_complexd)) //128
		cplx = 2;
	else if (s == sizeof(unsigned char)) //8
		cplx = 3;
}

#include <iostream>
int bbMatlab::work(int no, gr_vector_const_void_star &i, gr_vector_void_star &o)
{
	unsigned int ninp = i.size();
	int n = 0;
	char buf[1024];
	if (cplx == 1)
	{
		const gr_complex *in;
		while (n < no)
		{
			for (unsigned int ip = 0; ip < ninp; ip++)
			{
				in = (const gr_complex *)i[ip];
				sprintf(buf, "%f\t%f\t", (double)in[n].real(), (double)in[n].imag());
				fwrite(buf, 1, strlen(buf), fp);
			}
			sprintf(buf, "\n");
			fwrite(buf, 1, strlen(buf), fp);
			n++;
		}
	}
	else if (cplx == 0)
	{
		const float *in;
		while (n < no)
		{
			for (unsigned int ip = 0; ip < ninp; ip++)
			{
				in = (const float *)i[ip];
				sprintf(buf, "%f\t", (double)in[n]);
				fwrite(buf, 1, strlen(buf), fp);
			}
			sprintf(buf, "\n");
			fwrite(buf, 1, strlen(buf), fp);
			n++;
		}
	}
	else if (cplx == 2)
	{
		const gr_complexd *in;
		while (n < no)
		{
			for (unsigned int ip = 0; ip < ninp; ip++)
			{
				in = (const gr_complexd *)i[ip];
				sprintf(buf, "%f\t%f\t", (double)in[n].real(), (double)in[n].imag());
				fwrite(buf, 1, strlen(buf), fp);
			}
			sprintf(buf, "\n");
			fwrite(buf, 1, strlen(buf), fp);
			n++;
		}
	}
	else if (cplx == 3)
	{
		const unsigned char *in;
		while (n < no)
		{
			for (unsigned int ip = 0; ip < ninp; ip++)
			{
				in = (const unsigned char *)i[ip];
				sprintf(buf, "%d\t", (int)in[n]);
				fwrite(buf, 1, strlen(buf), fp);
			}
			sprintf(buf, "\n");
			fwrite(buf, 1, strlen(buf), fp);
			n++;
		}
	}
	fflush(fp);
	return no;
}
