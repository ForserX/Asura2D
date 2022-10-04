#pragma once

namespace ark::entities
{
	// non-serializable, created for scheduled entities destroying
	struct garbage_flag
	{
		static constexpr uint16_t flag = 1 << 0;
	};
	
	// non-serializable
	struct non_serializable_flag
	{
		static constexpr uint16_t flag = 1 << 1;
	};
	
	// non-serializable, created for engine/base game entities, which requirement is to be stable 
	struct dont_free_after_reset_flag
	{
		static constexpr uint16_t flag = 1 << 2;
	};
	
	// serializable, for background 
	struct background_flag
	{
		static constexpr uint16_t flag = 1 << 3;
	};
	
	// serializable, for drawing
	struct drawable_flag
	{
		static constexpr uint16_t flag = 1 << 4;
	};
	
	// serializable, for ground specification
	struct ground_flag
	{
		static constexpr uint16_t flag = 1 << 5;
	};
	
	// serializable, for level specification
	struct level_flag
	{
		static constexpr uint16_t flag = 1 << 6;
	};					

	// serializable, for networking and delta serialize
	struct net_id_flag
	{
		static constexpr uint16_t flag = 1 << 7;
	};

	// serializable, for marking player as controlled by AI or player
	struct net_controlled_flag
	{
		static constexpr uint16_t flag = 1 << 8;
	};

	template <typename T>
	using detect_flag = decltype(T::flag);	

	template <typename T>
	constexpr bool is_flag_v = stl::is_detected<detect_flag, T>::value;

	struct network_component
	{
		uint16_t network_id : 12;
		uint16_t network_cluster : 4;
		uint16_t reserved;
	};

	struct draw_color_component
	{
		ImColor color;

		bool can_serialize_now() const
		{
			return true;
		}
		
		template<bool direct = false>
		void serialize(stl::stream_vector& data) const
		{
			uint32_t value = color;
			stl::push_memory(data, value);
		}

		void deserialize(stl::stream_vector& data)
		{
			uint32_t value = 0;
			stl::read_memory(data, value);
			color = value;
		}
	};

	struct draw_gradient_component
	{
		ImColor first_color;
		ImColor second_color;

		bool can_serialize_now() const
		{
			return true;
		}

		template<bool direct = false>
		void serialize(stl::stream_vector& data) const
		{
			uint32_t value = first_color;
			stl::push_memory(data, value);
			
			value = second_color;
			stl::push_memory(data, value);
		}

		void deserialize(stl::stream_vector& data)
		{
			uint32_t value = 0;
			stl::read_memory(data, value);
			first_color = value;
			
			stl::read_memory(data, value);
			second_color = value;
		}
	};

	struct draw_texture_component
	{
		ImTextureID texture;

		bool can_serialize_now() const
		{
			return false;
		}

		template<bool direct = false>
		void serialize(stl::stream_vector& data) const
		{
			
		}

		void deserialize(stl::stream_vector& data)
		{
			
		}
	};

	struct scene_component
	{
		ark_float_vec2 position = {};

		bool can_serialize_now() const
		{
			return true;
		}

		void serialize(stl::stream_vector& data) const
		{
			stl::push_memory(data, position.x);
			stl::push_memory(data, position.y);
		}

		void deserialize(stl::stream_vector& data)
		{
			stl::read_memory(data, position.x);
			stl::read_memory(data, position.y);
		}
	};
	
	struct physics_body_component
	{
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
			ark_assert(body != nullptr, "Body is not freed yet. That means that you have a memory leak", {})
			
			const physics::body_parameters parameters(data);
			body = physics::schedule_creation(parameters);
		}
	};
	
	struct dynamic_visual_component
	{
		stl::vector<ark_float_vec2> points;
		
		bool can_serialize_now() const
		{
			return !points.empty();
		}

		void serialize(stl::stream_vector& data) const
		{
			size_t size_to_write = points.size();
			stl::push_memory(data, size_to_write);
			for (const auto& point : points) {
				stl::push_memory(data, point);
			}
		}

		void deserialize(stl::stream_vector& data)
		{
			size_t size_to_read = 0;
			stl::read_memory(data, size_to_read);
			
			points.resize(size_to_read);
			for (size_t i = 0; i < size_to_read; i++) {
				stl::read_memory(data, points[i]);
			}
		}
	};

#define DECLARE_SERIALIZABLE_TYPES \
	background_flag, \
	drawable_flag, \
	ground_flag, \
	level_flag, \
	net_id_flag, \
	net_controlled_flag, \
	draw_color_component, \
	draw_gradient_component, \
	draw_texture_component, \
	scene_component, \
	physics_body_component, \
	dynamic_visual_component

#define DECLASE_NON_SERIALIZABLE_TYPES \
	garbage_flag, \
	non_serializable_flag, \
	dont_free_after_reset_flag 
}
