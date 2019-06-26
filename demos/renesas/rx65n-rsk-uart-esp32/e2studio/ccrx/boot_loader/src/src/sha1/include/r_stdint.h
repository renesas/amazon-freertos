/* Copyright,2006 RENESAS SOLUTIONS CORPORATION ALL RIGHT RESERVED */

/* $Id: //depot/cryptogram/include/r_stdint.h#2 $ */
/* $Date: 2010/02/28 $ */

#ifndef __r_stdint_h__
#define __r_stdint_h__

#include <limits.h>

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed long int32_t;
typedef unsigned long uint32_t;
//typedef signed long long int64_t;
//typedef unsigned long long uint64_t;

#if UINT_MAX == 0xffffU
typedef int16_t	natural_int_t;
#else
typedef int32_t	natural_int_t;
#endif

#endif