#include "pch.h"

using namespace Asura;

void Threads::Init()
{
#ifdef OS_WINDOWS
	timeBeginPeriod(1);
#endif
}

void Threads::Destroy()
{
#ifdef OS_WINDOWS
	timeEndPeriod(1);
#endif
}

void Threads::SetAffinity(void* handle, int64_t Core)
{
#ifdef OS_WINDOWS
	auto mask = (static_cast<DWORD_PTR>(1) << Core); 
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

void Asura::Threads::sync_sleep()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void Threads::switch_context()
{
	for (int i = 0; i < 256; i++)
	{
		nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
		nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
		nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
		nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
	}
}