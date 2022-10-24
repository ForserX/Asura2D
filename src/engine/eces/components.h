#pragma once

namespace ark::entities
{
	// non-serializable, created for scheduled entities destroying
	struct garbage_flag
	{
		static constexpr net::flag_t flag = 1 << 0;
	};
	
	// non-serializable
	struct non_serializable_flag
	{
		static constexpr net::flag_t flag = 1 << 1;
	};
	
	// non-serializable, created for engine/base game entities, which requirement is to be stable 
	struct dont_free_after_reset_flag
	{
		static constexpr net::flag_t flag = 1 << 2;
	};
	
	// serializable, for background 
	struct background_flag
	{
		static constexpr net::flag_t flag = 1 << 3;
	};
	
	// serializable, for drawing
	struct drawable_flag
	{
		static constexpr net::flag_t flag = 1 << 4;
	};
	
	// serializable, for ground specification
	struct ground_flag
	{
		static constexpr net::flag_t flag = 1 << 5;
	};
	
	// serializable, for level specification
	struct level_flag
	{
		static constexpr net::flag_t flag = 1 << 6;
	};					

	// serializable, for networking and delta serialize
	struct net_id_flag
	{
		static constexpr net::flag_t flag = 1 << 7;
	};

	// serializable, for marking player as controlled by AI or player
	struct net_controlled_flag
	{
		static constexpr net::flag_t flag = 1 << 8;
	};

    constexpr uint32_t last_flag_index = 8;

	/*
	#define DECLARE_COMPONENT(name, ...) \
		struct name \
		{ \
			BOOST_HANA_DEFINE_STRUCT(name, __VA_ARGS__); \
		};

	#define PUSH_MEMBER(val) \
		{ \
			const static stl::string val_string = stl::combine_string<decltype(val)>(#val); \
			string_map[val_string] = stl::stringify(val); \
		} \

	#define POP_MEMBER(val) \
		{ \
			const static stl::string val_string = stl::combine_string<decltype(val)>(#val); \
			if (string_map.contains(val_string)) \
				val = stl::unstringify<decltype(val)>(string_map.at(val_string)); \
		} \

		void string_serialize(stl::string_map& string_map)
		{
			PUSH_MEMBER(color);
		}

		void string_deserialize(stl::string_map& string_map)
		{
			POP_MEMBER(color);
		}
	*/

	struct draw_color_component
	{
		ImColor color;
	};

	struct draw_gradient_component
	{
		ImColor first_color;
		ImColor second_color;
	};

	struct draw_texture_component
	{
        resources::id_t texture_resource;
	};

	struct scene_component
	{
        math::fvec2 size;
        math::transform transform;
	};
	
    struct camera_component
    {
        float cam_zoom;
        math::transform cam_transform;
    };

	struct physics_body_component
	{
		static constexpr bool custom_serialize = true;
		physics::physics_body* body = nullptr;

		bool can_serialize_now() const
		{
			return body != nullptr;
		}

		void serialize(stl::stream_vector& data) const
		{
			const physics::body_parameters parameters = body->copy_parameters();
			parameters.serialize(data);
		}

		void deserialize(stl::stream_vector& data)
		{
			body = physics::schedule_creation(physics::body_parameters(data));
		}

		void string_serialize(stl::string_map& data)
		{
			const physics::body_parameters parameters = body->copy_parameters();
			parameters.string_serialize(data);
		}

		void string_deserialize(stl::string_map& data)
		{
			body = physics::schedule_creation(physics::body_parameters(data));
		}
	};
}
