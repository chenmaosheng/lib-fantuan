#include "easy_baseloop.h"

class ChatLoop : public EasyBaseLoop
{
public:
	virtual EasySession* _CreateSession() { return new EasySession; }
};
