#ifndef _H_TYPE
#define _H_TYPE

typedef	signed char			int8;
typedef unsigned char		uint8;
typedef short				int16;
typedef unsigned short		uint16;
typedef int					int32;
typedef unsigned int		uint32;
#ifndef WIN32
typedef long long			int64;
typedef unsigned long long	uint64;
#else
typedef __int64				int64;
typedef unsigned __int64	uint64;
#endif
typedef float				float32;
typedef double				float64;

#endif
