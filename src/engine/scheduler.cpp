#include "pch.h"

using namespace ark;

constexpr auto scheduler_period = std::chrono::milliseconds(50);

std::mutex scheduler_mutex = {};
std::unique_ptr<std::thread> scheduler_thread = {};
std::array<stl::function_set<scheduler::global_function>, scheduler::global_task_type::count_of_elems> global_func_map = {};
bool scheduler_destroyed = false;

void 
scheduler::init()
{
	using namespace std::chrono_literals;

	scheduler_thread = std::make_unique<std::thread>([]() {
		OPTICK_THREAD("scheduler thread")
		while (!scheduler_destroyed) {
			OPTICK_EVENT("scheduler tick")
			auto trigger = [](bool parallel, global_task_type task_type) {
				stl::function_set<scheduler::global_function> funcs_to_delete;
				auto trigger_all = [&](const scheduler::global_function& func) {
					if (!func()) {
						funcs_to_delete.insert(func);
					}
				};

				if (!global_func_map[task_type].empty()) {
					for (auto& func : global_func_map[task_type]) {
						trigger_all(func);
					}

					{
						std::scoped_lock<std::mutex> scope_lock(scheduler_mutex);
						for (const auto& elem : funcs_to_delete) {
							global_func_map[task_type].erase(elem);
						}
					}
				}
			};

			auto schedule_time = std::chrono::steady_clock::now() + 50ms;
            
			{
				OPTICK_EVENT("scheduler work")

				{
					OPTICK_EVENT("garbage collector")
					trigger(false, global_task_type::garbage_collector);
				}

				{
					OPTICK_EVENT("entity serializator")
					trigger(false, global_task_type::entity_serializator);
				}

				{
					OPTICK_EVENT("resource manager")
					trigger(false, global_task_type::resource_manager);
				}
			}

			{
				OPTICK_EVENT("scheduler wait")
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
	scheduler_thread->join();
	scheduler_thread.reset();
}

const scheduler::global_function&
scheduler::internal::schedule(global_task_type task_type, const global_function& func)
{
    std::scoped_lock<std::mutex> scope_lock(scheduler_mutex);
	return *global_func_map[task_type].insert(func).first;
}

void scheduler::unschedule(global_task_type task_type, const global_function& func)
{
	global_func_map[task_type].erase(func);
}
