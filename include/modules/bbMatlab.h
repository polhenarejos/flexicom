// $Id$
#ifndef _INC_BBMATLAB_H_
#define _INC_BBMATLAB_H_

#include <gr_sync_block.h>
#include <complex>

/*! \brief bbMatlab class is used to extract data from the system chain to post-process it in Matlab
*/
class bbMatlab : public gr_sync_block
{
	public:
		typedef boost::shared_ptr<bbMatlab> sptr;
	   /**
       * The creation of bbMatlab requires 2 parameters: 
       * @param f: the name of the file where the samples are going to be stored.
       * @param s: the size of the data to be stored. 3 possible values: float, double (float and double format).
       */
		static sptr Create(std::string, size_t=sizeof(gr_complex));
		int work(int, gr_vector_const_void_star&, gr_vector_void_star&);
		~bbMatlab();
	
	private:
		bbMatlab(std::string, size_t = sizeof(gr_complex));
		FILE *fp;
		size_t s;
		int cplx;

};

#endif //_INC_BBMATLAB_H_
