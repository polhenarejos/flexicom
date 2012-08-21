// $Id: GUI.cc 1071 2010-12-13 10:26:46Z phenarejos $
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <gr_io_signature.h>
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
	gr_sync_block("bbMatlab", gr_make_io_signature(1, gr_io_signature::IO_INFINITE, _s), gr_make_io_signature(0, 0, 0)),
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
	if (s == sizeof(gr_complex)) //32
		cplx = 1;
	else if (s == sizeof(float)) //16
		cplx = 0;
	else if (s == sizeof(gr_complexd)) //64
		cplx = 2;
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
	fflush(fp);
	return 0;
}
