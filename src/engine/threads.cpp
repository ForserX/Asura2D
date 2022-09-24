﻿#include "pch.h"

using namespace ark;

void
threads::init()
{
#ifdef _WIN32
	timeBeginPeriod(1);
#endif
}

void
threads::destroy()
{
#ifdef _WIN32
	timeEndPeriod(1);
#endif
}

void threads::set_thread_affinity(void* handle, int64_t core)
{
#ifdef _WIN32
	auto mask = (static_cast<DWORD_PTR>(1) << core); 
	SetThreadAffinityMask(handle, mask);
#else

#endif
	
}