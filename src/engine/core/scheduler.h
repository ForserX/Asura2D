#pragma once

namespace asura::scheduler
{
	using global_function = fu2::function<bool() const>;

	enum global_task_type : size_t
	{
		resource_manager,
		garbage_collector,
		entity_serializator,

		count_of_elems
	};

	namespace internal
	{
		const global_function& schedule(global_task_type task_type, const global_function& func);
	}

	void init();
	void destroy();

	const void schedule(global_task_type task_type, auto&& func)
	{
		internal::schedule(task_type, global_function(func));
	}

	void unschedule(global_task_type task_type, const global_function& func);
}

constexpr float target_scheduler_tps = 50.f;
extern float scheduler_delta;
extern float scheduler_real_delta;
