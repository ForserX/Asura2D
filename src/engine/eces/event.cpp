#include "pch.h"

using namespace Asura;

event::internal::event_callback_storage events = {};

void event::Init()
{
}

void event::Tick()
{
}

void event::Destroy()
{
}

void event::create(stl::string_view name, int64_t parameters_count)
{
	game_assert(parameters_count >= 0 && parameters_count <= internal::max_parameters_count, "invalid parameters count", return);
	game_assert(!exists(name), "event already exists", return);
	
	events[name.data()] = {};
}

void event::remove(stl::string_view name)
{
	if (events.contains(name.data()))
	{
		events.erase(name.data());
	}
	
	game_assert(false, "event doesn't exist", return);
}

bool event::exists(stl::string_view name)
{
	return events.contains(name.data());
}

void event::internal::subscribe(stl::string_view name, const callback& sub_callback)
{
	auto& subscribers = events[name.data()];
	game_assert(!subscribers.contains(sub_callback), "callback already subscribed", return);
	subscribers.insert(sub_callback);
}

void event::internal::unsubscribe(stl::string_view name, const callback& sub_callback)
{
	auto& subscribers = events[name.data()];
	subscribers.erase(sub_callback);
}

void event::internal::trigger(stl::string_view name, parameter callback_parameter_1, parameter callback_parameter_2)
{
	game_assert(events.contains(name.data()), "event doesn't exists", return);
	const auto& subscribers = events[name.data()];

	for (auto& subscriber_callback : subscribers) 
	{
		if (subscriber_callback.index() == 0) 
		{
			callback_0 current_callback = std::get<callback_0>(subscriber_callback);
			current_callback();
		} 
		else if (subscriber_callback.index() == 1) 
		{
			callback_1 current_callback = std::get<callback_1>(subscriber_callback);
			current_callback(callback_parameter_1);
		} 
		else if (subscriber_callback.index() == 2)
		{
			callback_2 current_callback = std::get<callback_2>(subscriber_callback);
			current_callback(callback_parameter_1, callback_parameter_2);
		}
	}
}
