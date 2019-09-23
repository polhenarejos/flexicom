// $Id$                      
                                                                           
#ifndef _INC_TIMING_H_
#define _INC_TIMING_H_

#include <gnuradio/sync_decimator.h>

/*! \brief Timing block performs a maximum likelihood timing synchronization operation

*  According to the idea that appears in section 7.5.1 of the book: "Synchronization Techniques for Digital Receivers", Mengali, D'Andrea, 1997, 
*  in the derivation of the ML detector says that the optimum timing estimate corresponds to the sampling epoch tau that maximizes the energy of the 
*  sequence {y(iT + tau)}.
*/
                                                               
class Timing : public gr::sync_decimator
{
	public:
		typedef boost::shared_ptr<Timing> sptr;
	   /**
       * The creation of the Timing requires 2 parameters: 
       * @param _symbols:  the number of symbols needed to perform the average and obtain the sampling epoch tau.
       * @param _decimation: the decimation rate that has to be applied as a consequence of the oversampling factor applied at transmission
       */
		static sptr Create(unsigned int, unsigned int = 1);
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &);

	private:
		Timing(unsigned int, unsigned int);
		int syms;
};                                                                         

#endif //_INC_TIMING_H_
