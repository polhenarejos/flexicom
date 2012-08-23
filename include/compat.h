// $Id$
#ifndef _INC_COMPAT_H_
#define _INC_COMPAT_H_

#ifdef _WIN
#define __attribute__(x)
#define PACK( __Declaration__ , __Name__ ) __pragma( pack(push, 1) ) ; typedef __Declaration__ __Name__ ; __pragma( pack(pop) )
#else
#define PACK( __Declaration__ , __Name__ ) typedef __Declaration__ __attribute__((__packed__)) __Name__
#endif

#endif //_INC_COMPAT_H_
