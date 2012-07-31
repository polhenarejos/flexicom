// $Id$

#include "SHM.h"
#include <gr_io_signature.h>

//Source
template <class T>
SHMSource<T>::SHMSource(const char *_name) :
	gr_sync_block("SHMSource", gr_make_io_signature(0, 0, 0), gr_make_io_signature(1, 1, sizeof(T))),
	name(_name)
{
	//boost::interprocess::shared_memory_object::remove(name);
	shm = new boost::interprocess::shared_memory_object(boost::interprocess::open_only, name, boost::interprocess::read_write);
	shm->truncate(sizeof(SHMMemory<T>));
	region = new boost::interprocess::mapped_region(*shm, boost::interprocess::read_write);
	mem = (SHMMemory<T> *)region->get_address();
}
template <class T>
SHMSource<T>::~SHMSource()
{
	delete region;
	delete shm;
	boost::interprocess::shared_memory_object::remove(name);
}
template <class T>
typename SHMSource<T>::sptr SHMSource<T>::Create(const char *_name)
{
	return sptr(new SHMSource<T>(_name));
}
template <class T>
int SHMSource<T>::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	T *out = (T *)_o[0];
	for (int n = 0; n < no; n++)
	{
		mem->Stored.wait();
		mem->mtx.wait();
		out[n] = mem->data[mem->rp];
		mem->rp = (mem->rp+1)%SHMMemory<T>::Size;
		mem->mtx.post();
		mem->Empty.post();
	}
	return no;
}

//Sink
template <class T>
SHMSink<T>::SHMSink(const char *_name) :
	gr_sync_block("SHMSink", gr_make_io_signature(1, 1, sizeof(T)), gr_make_io_signature(0, 0, 0)),
	name(_name)
{
	boost::interprocess::shared_memory_object::remove(name);
	shm = new boost::interprocess::shared_memory_object(boost::interprocess::open_or_create, name, boost::interprocess::read_write);
	shm->truncate(sizeof(SHMMemory<T>));
	region = new boost::interprocess::mapped_region(*shm, boost::interprocess::read_write);
	mem = new (region->get_address()) SHMMemory<T>;
}
template <class T>
SHMSink<T>::~SHMSink()
{
	delete region;
	delete shm;
	boost::interprocess::shared_memory_object::remove(name);
}
template <class T>
typename SHMSink<T>::sptr SHMSink<T>::Create(const char *_name)
{
	return sptr(new SHMSink<T>(_name));
}
template <class T>
int SHMSink<T>::work(int no, gr_vector_const_void_star &_i, gr_vector_void_star &_o)
{
	const T *in = (const T *)_i[0];
	for (int n = 0; n < no; n++)
	{
		mem->Empty.wait();
		mem->mtx.wait();
		mem->data[mem->wp] = in[n];
		mem->wp = (mem->wp+1)%SHMMemory<T>::Size;
		mem->mtx.post();
		mem->Stored.post();
	}
	consume_each(no);
	return 0;
}
