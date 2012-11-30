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
		static void Decode(const int *, int *, int, int);
		static unsigned int bi2dec(int *, unsigned int);
		                                                                   
	private:                                                               
		Bi2De(unsigned int);                                                                                                           
};                                                                         
                                                                           
#endif //_INC_BI2DE_H_                                                     
