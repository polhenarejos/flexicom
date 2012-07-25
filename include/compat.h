// $Id$
#ifndef _INC_COMPAT_H_
#define _INC_COMPAT_H_

#ifdef _WIN
#define __attribute__(x)
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ ; __pragma( pack(pop) )
#else
#define PACK( __Declaration__ ) __Declaration__ 
#endif

#endif //_INC_COMPAT_H_
