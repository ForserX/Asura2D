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

	template <typename T>
	using detect_flag = decltype(T::flag);	

	struct draw_color_component
	{
		ImColor color;

		bool can_serialize_now() const
		{
			return true;
		}
		
		void serialize(stl::stream_vector& data) const
		{
			uint32_t value = color;
			write_memory(data, value);
		}

		void deserialize(stl::stream_vector& data)
		{
			uint32_t value = 0;
			read_memory(data, value);
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
		
		void serialize(stl::stream_vector& data) const
		{
			uint32_t value = first_color;
			write_memory(data, value);
			
			value = second_color;
			write_memory(data, value);
		}

		void deserialize(stl::stream_vector& data)
		{
			uint32_t value = 0;
			read_memory(data, value);
			first_color = value;
			
			read_memory(data, value);
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
		
		void serialize(stl::stream_vector& data) const
		{
			
		}

		void deserialize(stl::stream_vector& data)
		{
			
		}
	};

	struct scene_component
	{
		ark_float_vec2 position;

		bool can_serialize_now() const
		{
			return true;
		}
		
		void serialize(stl::stream_vector& data) const
		{
			write_memory(data, position.x);
			write_memory(data, position.y);
		}

		void deserialize(stl::stream_vector& data)
		{
			read_memory(data, position.x);
			read_memory(data, position.y);
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
	
	struct visual_component
	{
		stl::vector<ark_float_vec2> points;
		
		bool can_serialize_now() const
		{
			return !points.empty();
		}
		
		void serialize(stl::stream_vector& data) const
		{
			size_t size_to_write = points.size();
			write_memory(data, size_to_write);
			for (const auto& point : points) {
				write_memory(data, point);
			}
		}

		void deserialize(stl::stream_vector& data)
		{
			size_t size_to_read = 0;
			read_memory(data, size_to_read);
			
			points.resize(size_to_read);
			for (size_t i = 0; i < size_to_read; i++) {
				read_memory(data, points[i]);
			}
		}
	};

	using variant_type = stl::variant<
		garbage_flag,
		non_serializable_flag,
		dont_free_after_reset_flag,
		background_flag,
		drawable_flag,
		ground_flag,
		level_flag,
		net_id_flag,
		draw_color_component,
		draw_gradient_component,
		draw_texture_component,
		scene_component,
		physics_body_component,
		visual_component
	>;
}
