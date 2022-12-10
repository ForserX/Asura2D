#include "pch.h"

using namespace asura;

void threads::init()
{
#ifdef OS_WINDOWS
	timeBeginPeriod(1);
#endif
}

void threads::destroy()
{
#ifdef OS_WINDOWS
	timeEndPeriod(1);
#endif
}

void threads::set_thread_affinity(void* handle, int64_t core)
{
#ifdef OS_WINDOWS
	auto mask = (static_cast<DWORD_PTR>(1) << core); 
	SetThreadAffinityMask(handle, mask);
#else

#endif
	
}

inline void nop()
{
#ifdef OS_WINDOWS
	__nop();
#else
	__asm__ __volatile__("nop");
#endif
}

void asura::threads::sync_sleep()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void threads::switch_context()
{
	for (int i = 0; i < 256; i++)
	{
		nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
		nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
		nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
		nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
	}
}