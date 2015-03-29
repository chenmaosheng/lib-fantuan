#ifndef _H_EASY_CONTEXT_POOL
#define _H_EASY_CONTEXT_POOL

#include "common.h"

struct EasyContext;
class EasyContextPool
{
#ifdef WIN32
public:
	// initialize context pool with initial input size and output size
	EasyContextPool(uint32 input_buffer_size=MAX_INPUT_BUFFER, uint32 output_buffer_size=MAX_OUTPUT_BUFFER);
	// destroy context pool
	~EasyContextPool();

	// pop a context from input and output pool
	EasyContext*	PopInputContext();
	EasyContext*	PopOutputContext();
	// push a context into input and output pool
	void			PushInputContext(EasyContext*);
	void			PushOutputContext(EasyContext*);

	// pop a context, but only return its buffer
	char*			PopOutputBuffer();
	// push a context, use its buffer to point to the context
	void			PushOutputBuffer(char* buffer);

public:
	uint32			input_buffer_size_;			// input pool size
	uint32			input_context_count_;		// used input count
	uint32			output_buffer_size_;		// output pool size
	uint32			output_context_count_;		// used output count

	SLIST_HEADER	input_context_pool_;	// SList of input pool
	SLIST_HEADER	output_context_pool_;	// SList of output pool
#endif
};



#endif