#include "pch.h"

using namespace Asura;

void Threads::Init()
{
	Threads::SetName("Asura: Primary Thread");
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

void Threads::SetAffinity(std::thread& handle, int64_t Core)
{
#ifdef OS_WINDOWS
	auto mask = (static_cast<DWORD_PTR>(1) << Core); 
	SetThreadAffinityMask(handle.native_handle(), mask);
#elif defined(OS_LINUX) || defined(OS_BSD)
	//sched_setaffinity(/* need get pid */, sizeof(cpu_set_t), &mask);
#endif
	
}

void Asura::Threads::SetName(stl::string_view Name)
{
	if (!Debug::dbg_atttached())
		return;

#ifdef OS_WINDOWS
	constexpr size_t cSize = 64;
	wchar_t wc[cSize];
	mbstowcs(wc, Name.data(), cSize);

	SetThreadDescription(GetCurrentThread(), wc);
#elif defined(OS_LINUX) || defined(OS_BSD)
	pthread_setname_np(pthread_self(), Name.data());
#elif defined(OS_MACOS)
	pthread_setname_np(Name.data());
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

void Asura::Threads::SyncCurrentThread()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void Asura::Threads::Wait(size_t Time)
{
#ifdef OS_WINDOWS
	if (Time == 1)
	{
		_mm_pause();
	}
	else
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(Time));
	}
#else
	std::this_thread::sleep_for(std::chrono::milliseconds(Time));
#endif
}

void Threads::SwitchContext()
{
	for (int i = 0; i < 256; i++)
	{
		nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
		nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
		nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
		nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
	}
}