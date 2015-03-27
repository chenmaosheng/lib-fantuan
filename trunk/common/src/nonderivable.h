#ifndef _H_NONDERIVABLE
#define _H_NONDERIVABLE

namespace Fantuan
{

class NonDerivableHelper
{
private:
	NonDerivableHelper(){}
	friend class NonDerivable;
};

class NonDerivable : private virtual NonDerivableHelper
{
private:
	NonDerivable(){}
};

}

#endif

