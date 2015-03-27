#ifndef _H_NONCOPYABLE
#define _H_NONCOPYABLE

namespace Fantuan
{

class NonCopyable
{
protected:
	NonCopyable(){}
	~NonCopyable(){}

private:
	NonCopyable(const NonCopyable&);
	const NonCopyable& operator=(const NonCopyable&);
};

}

#endif

