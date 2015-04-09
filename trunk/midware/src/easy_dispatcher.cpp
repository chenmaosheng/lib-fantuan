#include "easy_dispatcher.h"
#include "easy_stream.h"

EasyDispatcher::Func* EasyDispatcher::m_pFuncList;

bool EasyDispatcher::OnPacketReceived(void* pSession, uint16 iTypeId, uint16 iLen, const char *pBuf)
{
	LOG_DBG(_T("FuncType=%d Len=%d"), iTypeId, iLen);
	InputStream stream(iLen, pBuf);	// put buffer into stream
	return m_pFuncList[iTypeId](pSession, stream);
}
