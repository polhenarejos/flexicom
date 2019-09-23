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

#ifndef _INC_SHM_H_
#define _INC_SHM_H_

#include <gnuradio/sync_block.h>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

template <class _T>
struct SHMMemory
{
	enum { Size = 4096*4 };
	SHMMemory() : mtx(1), Empty(Size), Stored(0), rp(0), wp(0) {}
	boost::interprocess::interprocess_semaphore mtx, Empty, Stored;
	unsigned int rp, wp;
	_T data[Size];
};
template <class _T>
class SHMSource : public gr::sync_block
{
	public:
		typedef boost::shared_ptr<SHMSource<_T> > sptr;
		static sptr Create(const char *);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		~SHMSource();
		
	private:
		SHMSource(const char *);
		boost::interprocess::shared_memory_object *shm;
		boost::interprocess::mapped_region *region;
		SHMMemory<_T> *mem;
		const char *name;
};

template <class _T>
class SHMSink : public gr::sync_block
{
	public:
		typedef boost::shared_ptr<SHMSink<_T> > sptr;
		static sptr Create(const char *);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);
		~SHMSink();
		
	private:
		SHMSink(const char *);
		boost::interprocess::shared_memory_object *shm;
		boost::interprocess::mapped_region *region;
		SHMMemory<_T> *mem;
		const char *name;
};

#endif //_INC_SHM_H_
