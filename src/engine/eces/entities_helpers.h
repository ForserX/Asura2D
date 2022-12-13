#pragma once

namespace Asura::Entities
{
	namespace internal
	{
		Registry& GetRegistry();

		void process_entities(auto&& func, uint8_t state)
		{
			using namespace std::chrono_literals;
			while (serialization_state != entities_state::idle)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			serialization_state = state;
			func();
			serialization_state = entities_state::idle;
		}
	}

	template<entities_state state = entities_state::viewing, typename... Args>
	void Access(auto&& func, Args&&...args)
	{
		{
			OPTICK_EVENT("waiting for write access");
			while (serialization_state != entities_state::idle)
			{
				Threads::SwitchContext();
			}
		}

		if (serialization_state == entities_state::idle)
		{
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
	void AccessView(auto&& func, Args&&...args)
	{
		{
			OPTICK_EVENT("waiting for view access");
			while (serialization_state != entities_state::idle && serialization_state != entities_state::viewing)
			{
				Threads::SwitchContext();
			}
		}

		if (serialization_state == entities_state::idle)
		{
			serialization_state = entities_state::viewing;
		}

		serialization_ref_counter++;
		func(std::forward<Args>(args)...);
		serialization_ref_counter--;

		if (serialization_ref_counter == 0)
		{
			serialization_state = entities_state::idle;
		}
	}

	template<typename Type, typename... Args>
	void AddField(const entt::entity& entt, Args &&...args)
	{
		internal::GetRegistry().Get().emplace<Type>(entt, std::forward<Args>(args)...);
	}

	template<typename Type, typename... Args>
	void AddField(const EntityView& entt, Args &&...args)
	{
		AddField<Type, Args...>(entt.Get(), std::forward<Args>(args)...);
	}

	template<typename Type>
	void EraseField(const entt::entity& entt)
	{
		internal::GetRegistry().Get().remove<Type>(entt);
	}

	template<typename Type>
	void EraseField(const EntityView& entt)
	{
		EraseField<Type>(entt.Get());
	}

	template<typename... Args>
	auto GetView()
	{
		auto view = internal::GetRegistry().Get().view<Args...>(entt::exclude<garbage_flag>);
		return view;
	}

	template<typename Type>
	auto TryGet(const entt::entity ent)
	{
		return internal::GetRegistry().Get().try_get<Type>(ent);
	}

	template<typename Type>
	auto TryGet(const EntityView& ent)
	{
		return TryGet<Type>(ent.Get());
	}

	template<typename Type>
	auto Get(entt::entity ent)
	{
		return internal::GetRegistry().Get().get<Type>(ent);
	}

	template<typename Type>
	auto Get(const EntityView& ent)
	{
		return get<Type>(ent.Get());
	}

	template<typename... Args>
	bool Contains(const entt::entity ent)
	{
		return internal::GetRegistry().Get().all_of<Args...>(ent);
	}

	template<typename... Args>
	bool Contains(const EntityView& ent)
	{
		return Contains<Args...>(ent.Get());
	}

	template<typename... Args>
	bool ContainsAny(const entt::entity ent)
	{
		return internal::GetRegistry().Get().any_of<Args...>(ent);
	}

	template<typename... Args>
	bool ContainsAny(const EntityView& ent)
	{
		return ContainsAny<Args...>(ent.Get());
	}
}
