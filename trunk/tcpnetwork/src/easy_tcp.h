#ifndef _H_EASY_TCP
#define _H_EASY_TCP

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

EASYDLL void	InitEasyTCP();
EASYDLL void	DestroyEasyTCP();
EASYDLL void*	CreateAcceptor(uint32 ip, uint16 port, uint32 count, bool (__stdcall*)(ConnID), void (__stdcall*)(ConnID), void (__stdcall*)(ConnID, uint32, char*), void (__stdcall*)(void*));
EASYDLL void	StartAcceptor(void*);

#ifdef __cplusplus
}
#endif
#endif
