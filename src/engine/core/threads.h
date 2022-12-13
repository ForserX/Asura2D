#pragma once

namespace Asura::Threads
{
	void Init();
	void Destroy();

	void SetAffinity(void* handle, int64_t Core);

	void SyncCurrentThread();
	void Wait();
	void SwitchContext();
}
