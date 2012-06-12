#ifndef _INC_BBMATLAB_H_
#define _INC_BBMATLAB_H_

#include <gr_sync_block.h>
#include <complex>

class bbMatlab : public gr_sync_block
{
	public:
		typedef boost::shared_ptr<bbMatlab> sptr;
		static sptr Create(std::string, size_t=sizeof(gr_complex));
		int work(int, gr_vector_const_void_star&, gr_vector_void_star&);
		~bbMatlab();
	
	private:
		bbMatlab(std::string, size_t = sizeof(gr_complex));
		FILE *fp;
		size_t s;
		int cplx;

};

#endif /* _INC_BBMATLAB_H_ */
