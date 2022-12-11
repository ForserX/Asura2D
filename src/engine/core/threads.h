#pragma once

namespace Asura::Threads
{
	void Init();
	void Destroy();

	void SetAffinity(void* handle, int64_t Core);

	void sync_sleep();
	void switch_context();
}
