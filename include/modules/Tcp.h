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

#ifndef _INC_TCP_H_
#define _INC_TCP_H_

#include <gnuradio/hier_block2.h>
#include <gnuradio/io_signature.h>

class TcpIO : public gr::hier_block2
{
	protected:
		TcpIO(const char *, unsigned short port, bool, gr::io_signature::sptr, gr::io_signature::sptr);
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
