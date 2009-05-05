#ifndef _yos_common_h
#define _yos_common_h

#define NULL 0

typedef unsigned char byte;
typedef unsigned short uint16;
typedef unsigned long uint32;

#define min(a,b) ({									\
			typeof(a) _a = (a); typeof(b) _b = (b);	\
			_a > _b ? _b : _a;						\
		})

#define max(a,b) ({									\
			typeof(a) _a = (a); typeof(b) _b = (b);	\
			_a > _b ? _a : _b;						\
		})

#define __likely(e)  __builtin_expect(!!(e), 1)
#define __unlikely(e)  __builtin_expect(!!(e), 0)

#define INSET(var, low, hig)  (((var) >= (low)) && ((var) <= (hig)))
#define INSET_EXCLUDE(var, low, hig)  (((var) > (low)) && ((var) < (hig)))

#endif

