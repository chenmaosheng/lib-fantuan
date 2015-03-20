#ifndef _H_EASY_BUF
#define _H_EASY_BUF

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _EasyBuf
{
	void*			data_;
	void* volatile	tail_;
	size_t			size_;
	size_t			curr_size_;
}EasyBuf;

EasyBuf*	CreateEasyBuf(size_t max_size);
void		DestroyEasyBuf(EasyBuf*);
bool		PushToEasyBuf(EasyBuf*, const void*, size_t);
size_t		PopFromEasyBuf(EasyBuf*, void*, size_t);

#ifdef __cplusplus
}
#endif

#endif

