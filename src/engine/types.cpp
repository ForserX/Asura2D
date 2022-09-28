#include "pch.h"

void* ark_alloc(size_t size_to_alloc)
{
#ifdef MIMALLOC_ENABLED
	return mi_malloc(size_to_alloc);
#else
	return malloc(size_to_alloc);
#endif
}

void ark_free(void* ptr)
{
#ifdef MIMALLOC_ENABLED
	mi_free(ptr);
#else
	free(ptr);
#endif
}