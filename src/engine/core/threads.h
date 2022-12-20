#pragma once

namespace Asura::Threads
{
	void Init();
	void Destroy();

	void SetAffinity(std::thread& handle, int64_t Core);
	void SetName(stl::string_view Name);

	void SyncCurrentThread();
	void Wait(size_t time = 1);
	void SwitchContext();
}
