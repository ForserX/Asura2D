#pragma once

namespace asura::threads
{
	void init();
	void destroy();

	void set_thread_affinity(void* handle, int64_t core);

	void sync_sleep();
	void switch_context();
}
