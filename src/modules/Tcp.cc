#include "Tcp.h"
#include <gr_file_descriptor_source.h>
#include <gr_file_descriptor_sink.h>
#include <sys/types.h>
//#include <io.h>
#ifdef _WIN
#include <fcntl.h>
#include <sys/stat.h>
#define dup(x) _dup(x)
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

TcpIO::TcpIO(const char *addr_str, unsigned short port, bool server, gr_io_signature_sptr in, gr_io_signature_sptr out) :
	gr_hier_block2("TcpIO", in, out)
{
	svfd = 0;
	if (server)
	{
#ifdef _WIN
		fd = _open("tmp-pipe", _O_CREAT | _O_TEMPORARY | _O_SEQUENTIAL | _O_RDWR | _O_BINARY, _S_IREAD | _S_IWRITE);
#else
		svfd = socket(AF_INET, SOCK_STREAM, 0);
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		memset(&(addr.sin_zero), 0, 8);
		addr.sin_port = htons(port);
		bind(svfd, (struct sockaddr *)&addr, sizeof(addr));
		listen(svfd, 4);
		fd = accept(svfd, 0, 0);
#endif
	}
	else
	{
#ifdef _WIN
		fd = _open("tmp-pipe", _O_CREAT | _O_TEMPORARY | _O_SEQUENTIAL | _O_RDWR | _O_BINARY, _S_IREAD | _S_IWRITE);
#else
		fd = socket(AF_INET, SOCK_STREAM, 0);
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		memset(&(addr.sin_zero), 0, 8);
		addr.sin_port = htons(port);
		::connect(fd, (struct sockaddr *)&addr, sizeof(addr));
#endif
	}
}
TcpIO::~TcpIO()
{
	close(fd);
}
TcpSource::TcpSource(size_t siz, const char *addr_str, unsigned short port, bool server) :
	TcpIO(addr_str, port, server, gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, 1, siz))
{
	gr_file_descriptor_source_sptr source = gr_make_file_descriptor_source(siz, dup(fd));
	connect(source, 0, self(), 0);
}
TcpSource::sptr TcpSource::Create(size_t siz, const char *addr_str, unsigned short port, bool server)
{
	return gnuradio::get_initial_sptr(new TcpSource(siz, addr_str, port, server));
}

TcpSink::TcpSink(size_t siz, const char *addr_str, unsigned short port, bool server) :
	TcpIO(addr_str, port, server, gr_make_io_signature(1, 1, siz), gr_make_io_signature(0, 0, 0))
{
	gr_file_descriptor_sink_sptr sink = gr_make_file_descriptor_sink(siz, dup(fd));
	connect(self(), 0, sink, 0);
}
TcpSink::sptr TcpSink::Create(size_t siz, const char *addr_str, unsigned short port, bool server)
{
	return gnuradio::get_initial_sptr(new TcpSink(siz, addr_str, port, server));
}
