#include "util.h"

namespace Fantuan
{

uint8 NumberPower(uint32 iNumber)
{
	uint8 i = 0;
	for (; i < 31 && iNumber != 0; ++i)
	{
		iNumber >>= i;
	}

	return i;
}

}

