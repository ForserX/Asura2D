#include "pch.h"

#ifdef OS_WINDOWS
#pragma comment(lib, "Winmm.lib")
#elif defined(OS_BSD)
#include <pthread_np.h>
#endif

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
	auto Mask = (static_cast<DWORD_PTR>(1) << Core);
	SetThreadAffinityMask(handle.native_handle(), Mask);
#elif defined(OS_SOLARIS)
//  stub
#elif defined(OS_APPLE_SERIES)
//	thread_affinity_policy_data_t Mask = { (int)Core };
//	thread_policy_set(pthread_mach_thread_np(handle.native_handle()), THREAD_AFFINITY_POLICY, (thread_policy_t)&Mask, 1);
#else
	cpu_set_t Mask;
	CPU_ZERO(&Mask);
	CPU_SET(Core, &Mask);

	pthread_setaffinity_np(handle.native_handle(), sizeof(cpu_set_t), &Mask);
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
#elif defined(OS_LINUX) || defined(OS_BSD) || defined(OS_SOLARIS)
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

Threads::ScopeCOThread::ScopeCOThread()
{
#ifdef OS_WINDOWS
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif
}

Threads::ScopeCOThread::~ScopeCOThread()
{
#ifdef OS_WINDOWS
	CoUninitialize();
#endif
}
