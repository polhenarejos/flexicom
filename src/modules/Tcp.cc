#include "Tcp.h"
#include <gr_file_descriptor_source.h>
#include <gr_file_descriptor_sink.h>
#include <sys/types.h>
#ifdef _WIN
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

TcpIO::TcpIO(const char *addr_str, unsigned short port, bool server, gr_io_signature_sptr in, gr_io_signature_sptr out) :
	gr_hier_block2("TcpIO", in, out)
{
	svfd = 0;
#ifdef _WIN
	WSADATA wsadata;
	WSAStartup(MAKEWORD(1, 1), &wsadata);
#endif
	if (server)
	{
		svfd = socket(AF_INET, SOCK_STREAM, 0);
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(port);
		bind(svfd, (struct sockaddr *)&addr, sizeof(addr));
		listen(svfd, 4);
		fd = accept(svfd, 0, 0);
	}
	else
	{
		fd = socket(AF_INET, SOCK_STREAM, 0);
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		addr.sin_port = htons(port);
		::connect(fd, (struct sockaddr *)&addr, sizeof(addr));
	}
}
TcpIO::~TcpIO()
{
#ifdef _WIN
	closesocket(fd);
#else
	close(fd);
#endif
	if (svfd != -1)
#ifdef _WIN
		closesocket(svfd);
	WSACleanup();
#else		
		close(svfd);
#endif
}
TcpSource::TcpSource(size_t siz, const char *addr_str, unsigned short port, bool server) :
	TcpIO(addr_str, port, server, gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, 1, siz))
{
	gr_file_descriptor_source_sptr source = gr_make_file_descriptor_source(siz, fd);
	connect(source, 0, self(), 0);
}
TcpSource::sptr TcpSource::Create(size_t siz, const char *addr_str, unsigned short port, bool server)
{
	return gnuradio::get_initial_sptr(new TcpSource(siz, addr_str, port, server));
}

TcpSink::TcpSink(size_t siz, const char *addr_str, unsigned short port, bool server) :
	TcpIO(addr_str, port, server, gr_make_io_signature(1, 1, siz), gr_make_io_signature(0, 0, 0))
{
	gr_file_descriptor_sink_sptr sink = gr_make_file_descriptor_sink(siz, fd);
	connect(self(), 0, sink, 0);
}
TcpSink::sptr TcpSink::Create(size_t siz, const char *addr_str, unsigned short port, bool server)
{
	return gnuradio::get_initial_sptr(new TcpSink(siz, addr_str, port, server));
}
