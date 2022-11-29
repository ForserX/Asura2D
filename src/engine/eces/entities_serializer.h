#pragma once

namespace asura::entities
{
	namespace internal
	{
		void serialize(stl::stream_vector& data);
		void deserialize(stl::stream_vector& data);

		void string_serialize(stl::tree_string_map& data);
		void string_deserialize(const stl::tree_string_map& data);
	}

	const std::chrono::nanoseconds& get_last_serialize_time();

	void deserialize_state(stl::string_view state_name);
	void serialize_state(stl::string_view state_name);

	void string_serialize_state(stl::string_view state_name);
	void string_deserialize_state(stl::string_view state_name);

	void serialize(stl::stream_vector& data);
	void deserialize(stl::stream_vector& data);

	void string_serialize(stl::tree_string_map& data);
	void string_deserialize(const stl::tree_string_map& data);

	template<typename T>
	struct custom_serializer;

	template<>
	struct custom_serializer<physics_body_component>
	{
		static_assert(stl::is_custom_serialize_v<physics_body_component>, "trying to serialize via custom serializer without required flag");

		static bool can_serialize_now(const physics_body_component& comp)
		{
			return comp.body != nullptr;
		}

		static void serialize(const physics_body_component& comp, stl::stream_vector& data)
		{
			const physics::body_parameters parameters = comp.body->copy_parameters();
			parameters.serialize(data);
		}

		static void deserialize(physics_body_component& comp, stl::stream_vector& data)
		{
			comp.body = physics::schedule_creation(physics::body_parameters(data));
		}

		static void string_serialize(const physics_body_component& comp, stl::string_map& data)
		{
			const physics::body_parameters parameters = comp.body->copy_parameters();
			parameters.string_serialize(data);
		}

		static void string_deserialize(physics_body_component& comp, stl::string_map& data)
		{
			comp.body = physics::schedule_creation(physics::body_parameters(data));
		}
	};
}

#define DECLARE_SERIALIZABLE_FLAGS \
    asura::entities::background_flag, \
    asura::entities::drawable_flag, \
    asura::entities::level_flag, \
	DECLARE_GAME_SERIALIZABLE_FLAGS

#define DECLARE_SERIALIZABLE_TYPES \
    asura::entities::draw_color_component, \
    asura::entities::draw_gradient_component, \
    asura::entities::draw_texture_component, \
    asura::entities::scene_component, \
    asura::entities::physics_body_component \
	DECLARE_GAME_SERIALIZABLE_TYPES

#define DECLARE_SERIALIZABLE_ENTITY_TYPES \
    DECLARE_SERIALIZABLE_FLAGS, \
    DECLARE_SERIALIZABLE_TYPES

#define DECLARE_NON_SERIALIZABLE_TYPES \
    asura::entities::garbage_flag, \
    asura::entities::non_serializable_flag, \
    asura::entities::dont_free_after_reset_flag \
	DECLARE_GAME_NON_SERIALIZABLE_ENTITY_TYPES

#define DECLARE_ENTITIES_TYPES \
    DECLARE_SERIALIZABLE_TYPES, \
    DECLARE_NON_SERIALIZABLE_TYPES

