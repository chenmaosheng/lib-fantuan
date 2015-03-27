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

	LocalString<64> ee("1234567890");
	LocalString<64> ff("abcdefghij");
	ee += "0987654321";
	printf("len=%d, data=%s\n", ee.strlen(), ee.c_str());
	ee += ff;
	printf("len=%d, data=%s\n", ee.strlen(), ee.c_str());
	ee = ff;
	printf("len=%d, data=%s\n", ee.strlen(), ee.c_str());

	String gg("thanks very much.");
	String hh("You are welcome");
	gg += hh;
	printf("len=%d, data=%s\n", gg.strlen(), gg.c_str());
	gg += "Hello, everyone";
	printf("len=%d, data=%s\n", gg.strlen(), gg.c_str());
	gg = hh;
	printf("len=%d, data=%s\n", gg.strlen(), gg.c_str());
}
