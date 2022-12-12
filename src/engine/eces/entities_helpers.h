#pragma once

namespace Asura::Entities
{
	namespace internal
	{
		registry& get_registry();

		void process_entities(auto&& func, uint8_t state)
		{
			using namespace std::chrono_literals;
			while (serialization_state != entities_state::idle) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			serialization_state = state;
			func();
			serialization_state = entities_state::idle;
		}
	}

	template<entities_state state = entities_state::viewing, typename... Args>
	void access(auto&& func, Args&&...args)
	{
		{
			OPTICK_EVENT("waiting for write access")
				while (serialization_state != entities_state::idle) {
					Threads::switch_context();
				}
		}

		if (serialization_state == entities_state::idle) {
			serialization_state = state;
		}

		serialization_ref_counter++;
		func(std::forward<Args>(args)...);
		serialization_ref_counter--;

		if (serialization_ref_counter == 0) {
			serialization_state = entities_state::idle;
		}
	}

	template<typename... Args>
	void access_view(auto&& func, Args&&...args)
	{
		{
			OPTICK_EVENT("waiting for view access")
				while (serialization_state != entities_state::idle && serialization_state != entities_state::viewing) {
					Threads::switch_context();
				}
		}

		if (serialization_state == entities_state::idle) {
			serialization_state = entities_state::viewing;
		}

		serialization_ref_counter++;
		func(std::forward<Args>(args)...);
		serialization_ref_counter--;

		if (serialization_ref_counter == 0) {
			serialization_state = entities_state::idle;
		}
	}

	template<typename Type, typename... Args>
	void add_field(const entt::entity& entt, Args &&...args)
	{
		internal::get_registry().get().emplace<Type>(entt, std::forward<Args>(args)...);
	}

	template<typename Type, typename... Args>
	void add_field(const entity_view& entt, Args &&...args)
	{
		add_field<Type, Args...>(entt.get(), std::forward<Args>(args)...);
	}

	template<typename Type>
	void erase_field(const entt::entity& entt)
	{
		internal::get_registry().get().remove<Type>(entt);
	}

	template<typename Type>
	void erase_field(const entity_view& entt)
	{
		erase_field<Type>(entt.get());
	}

	template<typename... Args>
	auto get_view()
	{
		auto view = internal::get_registry().get().view<Args...>(entt::exclude<garbage_flag>);
		return view;
	}

	template<typename Type>
	auto try_get(const entt::entity ent)
	{
		return internal::get_registry().get().try_get<Type>(ent);
	}

	template<typename Type>
	auto try_get(const entity_view& ent)
	{
		return try_get<Type>(ent.get());
	}

	template<typename Type>
	auto get(entt::entity ent)
	{
		return internal::get_registry().get().get<Type>(ent);
	}

	template<typename Type>
	auto get(const entity_view& ent)
	{
		return get<Type>(ent.get());
	}

	template<typename... Args>
	bool contains(const entt::entity ent)
	{
		return internal::get_registry().get().all_of<Args...>(ent);
	}

	template<typename... Args>
	bool contains(const entity_view& ent)
	{
		return contains<Args...>(ent.get());
	}

	template<typename... Args>
	bool contains_any(const entt::entity ent)
	{
		return internal::get_registry().get().any_of<Args...>(ent);
	}

	template<typename... Args>
	bool contains_any(const entity_view& ent)
	{
		return contains_any<Args...>(ent.get());
	}
}
