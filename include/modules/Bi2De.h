// $Id$                      
                                                                           
#ifndef _INC_BI2DE_H_                                                      
#define _INC_BI2DE_H_                                                      
                                                                           
#include <gr_sync_decimator.h>
                                                                                                                  
class Bi2De : public gr_sync_decimator                    
{                                                                          
	public:                                                                
		typedef boost::shared_ptr<Bi2De> sptr;                             
		static sptr Create(unsigned int);                                  
		int work(int, gr_vector_const_void_star &, gr_vector_void_star &); 
		~Bi2De();                                                          
		                                                                   
	private:                                                               
		Bi2De(unsigned int);                                                                                                           
};                                                                         
                                                                           
#endif //_INC_BI2DE_H_                                                     
