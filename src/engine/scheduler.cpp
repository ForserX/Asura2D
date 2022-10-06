#include "pch.h"

using namespace ark;

constexpr auto scheduler_period = std::chrono::milliseconds(50);

marl::mutex scheduler_mutex;
std::unique_ptr<std::jthread> scheduler_thread;
std::array<stl::function_set<scheduler::global_function>, scheduler::global_task_type::count_of_elems> global_func_map;
bool scheduler_destroyed = false;

void 
scheduler::init()
{
	using namespace std::chrono_literals;

	scheduler_thread = std::make_unique<std::jthread>([]() {
		OPTICK_THREAD("Scheduler thread")
		while (!scheduler_destroyed) {
			OPTICK_EVENT("Scheduler tick")
			auto trigger = [](bool parallel, global_task_type task_type) {
				stl::function_set<scheduler::global_function> funcs_to_delete;
				auto trigger_all = [&](const scheduler::global_function& func) {
					if (!func()) {
						funcs_to_delete.insert(func);
					}
				};

				if (!global_func_map[task_type].empty()) {
					if (parallel) {
						std::for_each(std::execution::par_unseq, global_func_map[task_type].begin(), global_func_map[task_type].end(), trigger_all);
					} else {
						std::for_each(std::execution::unseq, global_func_map[task_type].begin(), global_func_map[task_type].end(), trigger_all);
					}

					{
						marl::lock scope_lock(scheduler_mutex);
						for (const auto& elem : funcs_to_delete) {
							global_func_map[task_type].erase(elem);
						}
					}
				}
			};

			auto schedule_time = std::chrono::steady_clock::now() + 50ms;
			{
				OPTICK_EVENT("Scheduler work")

				{
					OPTICK_EVENT("Garbage collector")
					trigger(false, global_task_type::garbage_collector);
				}

				{
					OPTICK_EVENT("Entity serializator")
					trigger(false, global_task_type::entity_serializator);
				}

				{
					OPTICK_EVENT("Resource manager")
					trigger(false, global_task_type::resource_manager);
				}
			}

			{
				OPTICK_EVENT("Scheduler wait")
				std::this_thread::sleep_until(schedule_time);
			}
		}

		scheduler_destroyed = false;
	});
}

void 
scheduler::destroy()
{
	scheduler_destroyed = true;
}

const scheduler::global_function&
scheduler::internal::schedule(global_task_type task_type, const global_function& func)
{
	marl::lock scope_lock(scheduler_mutex);
	return *global_func_map[task_type].insert(func).first;
}

void scheduler::unschedule(global_task_type task_type, const global_function& func)
{
	global_func_map[task_type].erase(func);
}
