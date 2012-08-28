// $Id$                      
                                                                           
#ifndef _INC_BB_TIMING_ML_H_                                                      
#define _INC_BB_TIMING_ML_H_                                                      
                                                                           
#include <gr_block.h>
            
/*! \brief bb_timing_ML block performs a maximum likelihood timing synchronization operation

*  According to the idea that appears in section 7.5.1 of the book: "Synchronization Techniques for Digital Receivers", Mengali, D'Andrea, 1997, 
*  in the derivation of the ML detector says that the optimum timing estimate corresponds to the sampling epoch tau that maximizes the energy of the 
*  sequence {y(iT + tau)}.
*/
                                                                                                      
class bb_timing_ML : public gr_block                    
{                                       
	public:                                                                
		typedef boost::shared_ptr<bb_timing_ML> sptr; 
	   /**
       * The creation of the bb_timing_ML requires 2 parameters: 
       * @param _symbols:  the number of symbols needed to perform the average and obtain the sampling epoch tau.
       * @param _decimation: the decimation rate that has to be applied as a consequence of the oversampling factor applied at transmission
       */                            
		static sptr Create(unsigned int, unsigned int);                                  
		int general_work(int, gr_vector_int &,gr_vector_const_void_star&, gr_vector_void_star&);
		void forecast(int, gr_vector_int &);
		~bb_timing_ML();                                                          
		                                                                   
	private:                                                               
		bb_timing_ML(unsigned int, unsigned int);
		float energy ( float *, unsigned int);
		int symbols;
		int decimation;                                                                                                           
};                                                                         
                                                                           
#endif //_INC_BB_TIMING_ML_H_                                                     
