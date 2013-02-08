#include "base.h"
#include "local_string.h"
#include "dy_string.h"

using namespace Fantuan;

void StringTest1()
{
	LocalString<32> aa("I love you, yanxi");
	printf("len=%d, data=%s\n", aa.strlen(), aa.c_str());
	LocalString<32> bb;
	bb = "Hello, World";
	printf("len=%d, data=%s\n", bb.strlen(), bb.c_str());

	String cc("I love you, too");
	printf("len=%d, data=%s\n", cc.strlen(), cc.c_str());
	String dd;
	dd = "Hello, World, too";
	printf("len=%d, data=%s\n", dd.strlen(), dd.c_str());
	aa.strcpy(bb.c_str(), bb.strlen());
	printf("len=%d, data=%s\n", aa.strlen(), aa.c_str());
	cc.strcpy(dd);
	printf("len=%d, data=%s\n", cc.strlen(), cc.c_str());
	dd.clear();
	printf("len=%d, data=%s\n", dd.strlen(), dd.c_str());
}
