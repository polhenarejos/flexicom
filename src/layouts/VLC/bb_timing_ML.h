// $Id$                      
                                                                           
#ifndef _INC_BB_TIMING_ML_H_                                                      
#define _INC_BB_TIMING_ML_H_                                                      
                                                                           
#include <gr_block.h>
                                                                                                                  
class bb_timing_ML : public gr_block                    
{                                       
	public:                                                                
		typedef boost::shared_ptr<bb_timing_ML> sptr;                             
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
