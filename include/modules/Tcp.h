// $Id$
#ifndef _INC_TCP_H_
#define _INC_TCP_H_

#include <gr_hier_block2.h>
#include <gr_io_signature.h>

class TcpIO : public gr_hier_block2
{
	protected:
		TcpIO(const char *, unsigned short port, bool, gr_io_signature_sptr, gr_io_signature_sptr);
		~TcpIO();
		int svfd;
		int fd;
};
class TcpSource : public TcpIO
{
	public:
		typedef boost::shared_ptr<TcpSource> sptr;
		static sptr Create(size_t, const char *, unsigned short port, bool = true);
		
	private:
		TcpSource(size_t, const char *, unsigned short port, bool = true);
};
class TcpSink : public TcpIO
{
	public:
		typedef boost::shared_ptr<TcpSink> sptr;
		static sptr Create(size_t, const char *, unsigned short port, bool = false);
		
	private:
		TcpSink(size_t, const char *, unsigned short port, bool = false);
};
#endif //_INC_TCP_H_
