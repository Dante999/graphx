#ifndef UTIL_DEFINES_H
#define UTIL_DEFINES_H

#ifndef ARRAY_SIZE
#	define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

#ifndef MIN
#	define MIN(x,y) (x < y ? x : y)
#endif

#ifndef MAX 
#	define MAX(x,y) (x > y ? x : y)
#endif


#define MATH_CEIL(a,b) ((a / b) + ((a % b != 0) ? 1 : 0))






#ifndef UNUSED
#   define UNUSED(x) (void) x
#endif

#ifndef NOT_IMPLEMENTED
#	define NOT_IMPLEMENTED(x) fprintf(stderr,"%s:%s not implemented!\n",  __FUNCTION__, __LINE__)
#endif

#ifndef TODO
#	define TODO(x) 
#endif 

#endif // UTIL_DEFINES_H
