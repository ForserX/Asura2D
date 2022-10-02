#pragma once

namespace ark::threads
{
	void init();
	void destroy();
	void set_thread_affinity(void* handle, int64_t core);

	void switch_context();
}
