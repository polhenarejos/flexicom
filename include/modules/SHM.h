// $Id$

#ifndef _INC_SHM_H_
#define _INC_SHM_H_

#include <gr_sync_block.h>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

template <class _T>
struct SHMMemory
{
	enum { Size = 8192 };
	SHMMemory() : mtx(1), Empty(Size), Stored(0), rp(0), wp(0) {}
	boost::interprocess::interprocess_semaphore mtx, Empty, Stored;
	unsigned int rp, wp;
	_T data[Size];
};
template <class _T>
class SHMSource : public gr_sync_block
{
	public:
		typedef boost::shared_ptr<SHMSource<_T>> sptr;
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
class SHMSink : public gr_sync_block
{
	public:
		typedef boost::shared_ptr<SHMSink<_T>> sptr;
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
