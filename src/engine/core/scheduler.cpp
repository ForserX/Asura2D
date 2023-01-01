#include "pch.h"

using namespace Asura;

constexpr auto scheduler_period = std::chrono::milliseconds(static_cast<int>(target_scheduler_tps));

std::mutex scheduler_mutex = {};
std::unique_ptr<std::thread> scheduler_thread = {};
std::array<stl::vector<Scheduler::global_function>, Scheduler::global_task_type::count_of_elems> global_func_map;
bool scheduler_destroyed = false;

float scheduler_delta = 0.f;
float scheduler_real_delta = 0.f;

void Scheduler::Init()
{
	using namespace std::chrono_literals;

	scheduler_thread = std::make_unique<std::thread>([]()
		{
        std::chrono::nanoseconds last_scheduler_time = {};
        
		OPTICK_THREAD("Scheduler thread");
		while (!scheduler_destroyed)
		{
			OPTICK_EVENT("Scheduler Destroy");
			auto trigger = [](bool parallel, global_task_type task_type) 
			{
				stl::vector<Scheduler::global_function> funcs_to_delete;
				auto trigger_all = [&](const Scheduler::global_function& func) 
				{
					if (!func()) 
					{
						funcs_to_delete.push_back(func);
					}
				};

				auto& current_map = global_func_map[task_type];

				if (!current_map.empty()) 
				{
					for (const auto& func : current_map) 
					{
						trigger_all(func);
					}

					{
						size_t Iter = 0;
						std::scoped_lock<std::mutex> scope_lock(scheduler_mutex);
						for (const auto& elem : funcs_to_delete) 
						{
							current_map.erase(current_map.begin() + Iter);
							Iter++;
						}
					}
				}
			};

            auto previous_scheduler_time = last_scheduler_time;
            last_scheduler_time = std::chrono::steady_clock::now().time_since_epoch();
            auto delta_time = last_scheduler_time - previous_scheduler_time;
            scheduler_delta = static_cast<double>(delta_time.count()) / 1000000000.;
            
			auto schedule_time = (std::chrono::steady_clock::now() + 50ms);
            
			{
				OPTICK_EVENT("Scheduler work");

				{
					OPTICK_EVENT("garbage collector");
					trigger(false, global_task_type::garbage_collector);
				}

				{
					OPTICK_EVENT("entity serializator");
					trigger(false, global_task_type::entity_serializator);
				}

				{
					OPTICK_EVENT("resource manager");
					trigger(false, global_task_type::resource_manager);
				}
			}
        
            auto real_delta_time = std::chrono::steady_clock::now().time_since_epoch() - last_scheduler_time;
            scheduler_real_delta = static_cast<double>(real_delta_time.count()) / 1000000000.0;

			{
				OPTICK_EVENT("Scheduler wait");
				std::this_thread::sleep_until(schedule_time);
			}
		}

		scheduler_destroyed = false;
	});
}

void Scheduler::Destroy()
{
    scheduler_destroyed = true;
	scheduler_thread->join();
	scheduler_thread.reset();
}

size_t Scheduler::internal::Schedule(global_task_type task_type, const global_function& func)
{
    std::scoped_lock<std::mutex> scope_lock(scheduler_mutex);
	global_func_map[task_type].emplace_back(func);
	return global_func_map[task_type].size() - 1;
}

void Scheduler::Unschedule(global_task_type task_type, size_t func)
{
	global_func_map[task_type].erase(global_func_map[task_type].begin() + func);
}
