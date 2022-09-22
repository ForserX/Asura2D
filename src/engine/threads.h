#pragma once

namespace ark::threads
{
	void set_thread_affinity(void* handle, int64_t core);
}
