#pragma once

namespace ark::entities
{
	namespace internal
	{
		void serialize(stl::stream_vector& data);
		void deserialize(stl::stream_vector& data);
	}

	const std::chrono::nanoseconds& get_last_serialize_time();

	void deserialize_state(stl::string_view state_name);
	void serialize_state(stl::string_view state_name);

	void serialize(stl::stream_vector& data);
	void deserialize(stl::stream_vector& data);
}

#define DECLARE_SERIALIZABLE_FLAGS \
    ark::entities::background_flag, \
    ark::entities::drawable_flag, \
    ark::entities::ground_flag, \
    ark::entities::level_flag, \
    ark::entities::net_id_flag, \
    ark::entities::net_controlled_flag \
	DECLARE_GAME_SERIALIZABLE_FLAGS

#define DECLARE_SERIALIZABLE_TYPES \
    ark::entities::draw_color_component, \
    ark::entities::draw_gradient_component, \
    ark::entities::draw_texture_component, \
    ark::entities::scene_component, \
    ark::entities::physics_body_component \
	DECLARE_GAME_SERIALIZABLE_TYPES

#define DECLARE_SERIALIZABLE_ENTITY_TYPES \
    DECLARE_SERIALIZABLE_FLAGS, \
    DECLARE_SERIALIZABLE_TYPES

#define DECLARE_NON_SERIALIZABLE_TYPES \
    ark::entities::garbage_flag, \
    ark::entities::non_serializable_flag, \
    ark::entities::dont_free_after_reset_flag \
	DECLARE_GAME_NON_SERIALIZABLE_ENTITY_TYPES

#define DECLARE_ENTITIES_TYPES \
    DECLARE_SERIALIZABLE_TYPES, \
    DECLARE_NON_SERIALIZABLE_TYPES

