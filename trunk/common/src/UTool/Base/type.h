#ifndef _H_TYPE
#define _H_TYPE

namespace Fantuan
{

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

}

using Fantuan::int8;
using Fantuan::uint8;
using Fantuan::int16;
using Fantuan::uint16;
using Fantuan::int32;
using Fantuan::uint32;
using Fantuan::int64;
using Fantuan::uint64;
using Fantuan::float32;
using Fantuan::float64;

#endif

