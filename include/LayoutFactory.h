// $Id$
#ifndef _INC_LAYOUTFACTORY_H_
#define _INC_LAYOUTFACTORY_H_

#include <gr_uhd_usrp_source.h>
#include <gr_uhd_usrp_sink.h>
#include <gr_file_source.h>
#include <gr_file_sink.h>
#include <gr_udp_sink.h>
#include <gr_udp_source.h>
#include <gr_null_sink.h>
#include <gr_null_source.h>

#include "bbMatlab.h"
#include "Tcp.h"
#include "SHM.h"
#include <gr_sptr_magic.h>
class MainWindow;

class LayoutFactory : boost::noncopyable
{
	private:
		int devs;
		int dev;
		bool rx;
		
	protected:
		LayoutFactory(MainWindow *, int);
		MainWindow *mw;
		int radioID;
		
	public:
		SHMSink<gr_complex>::sptr shm_sink[8];
		gr_file_sink_sptr file_sink[8];
		bbMatlab::sptr matlab_sink[8];
		gr_null_sink_sptr null_sink[8];
		TcpSink::sptr tcp_sink[8];
		gr_udp_sink_sptr udp_sink[8];
		boost::shared_ptr<uhd_usrp_sink> usrp_sink;
			
		SHMSource<gr_complex>::sptr shm_source[8];
		gr_file_source_sptr file_source[8];
		gr_null_source_sptr null_source[8];
		TcpSource::sptr tcp_source[8];
		gr_udp_source_sptr udp_source[8];
		boost::shared_ptr<uhd_usrp_source> usrp_source;
		virtual const char *Name() = 0;
		typedef boost::shared_ptr<LayoutFactory> sptr;
		virtual void Run() = 0;
		virtual void Stop() = 0;
		int Sources();
		boost::shared_ptr<gr_basic_block> Source(int);
		int Sinks();
		boost::shared_ptr<gr_basic_block> Sink(int);
		void Setup();
};

#endif //_INC_LAYOUTFACTORY_H_
