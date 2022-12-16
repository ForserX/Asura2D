#pragma once

namespace Asura::Scheduler
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
		size_t Schedule(global_task_type task_type, const global_function& func);
	}

	void Init();
	void Destroy();

	inline size_t Schedule(global_task_type task_type, auto&& func)
	{
		return internal::Schedule(task_type, global_function(func));
	}

	void Unschedule(global_task_type task_type, size_t func);
}

constexpr float target_scheduler_tps = 50.f;