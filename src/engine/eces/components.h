#pragma once

namespace ark::entities
{
	// non-serializable, created for scheduled entities destroying
	struct garbage_flag
	{
		static constexpr net::flag_type flag = 1 << 0;
	};
	
	// non-serializable
	struct non_serializable_flag
	{
		static constexpr net::flag_type flag = 1 << 1;
	};
	
	// non-serializable, created for engine/base game entities, which requirement is to be stable 
	struct dont_free_after_reset_flag
	{
		static constexpr net::flag_type flag = 1 << 2;
	};
	
	// serializable, for background 
	struct background_flag
	{
		static constexpr net::flag_type flag = 1 << 3;
	};
	
	// serializable, for drawing
	struct drawable_flag
	{
		static constexpr net::flag_type flag = 1 << 4;
	};
	
	// serializable, for ground specification
	struct ground_flag
	{
		static constexpr net::flag_type flag = 1 << 5;
	};
	
	// serializable, for level specification
	struct level_flag
	{
		static constexpr net::flag_type flag = 1 << 6;
	};					

	// serializable, for networking and delta serialize
	struct net_id_flag
	{
		static constexpr net::flag_type flag = 1 << 7;
	};

	// serializable, for marking player as controlled by AI or player
	struct net_controlled_flag
	{
		static constexpr net::flag_type flag = 1 << 8;
	};

    constexpr uint32_t last_flag_index = 8;

	template <typename T>
	using detect_flag = decltype(T::flag);	

	template <typename T>
	constexpr bool is_flag_v = stl::is_detected<detect_flag, T>::value;

	struct network_data
	{
        net::id_type net_id;
	};

    struct network_component
    {
        network_data net_data;
        
        bool can_serialize_now() const
        {
            return true;
        }

        bool can_string_deserialize(stl::string_map& kv_storage) const
        {
            return (kv_storage.contains("net_id"));
        }
        
        void string_deserialize(stl::string_map& kv_storage)
        {
            net_data.net_id = std::stoul(kv_storage.at("net_id").data());
        }

        void string_serialize(stl::string_map& kv_storage) const
        {
            kv_storage["net_id"] = std::to_string(static_cast<uint32_t>(net_data.net_id));
        }

        void serialize(stl::stream_vector& data) const
        {
            stl::push_memory(data, net_data);
        }

        void deserialize(stl::stream_vector& data)
        {
            stl::read_memory(data, net_data);
        }
    };

	struct draw_color_component
	{
		ImColor color;

		bool can_serialize_now() const
		{
			return true;
		}

        bool can_string_deserialize(stl::string_map& kv_storage) const
        {
            return kv_storage.contains("color");
        }
        
		void string_deserialize(stl::string_map& kv_storage)
		{
            color = std::stoul(kv_storage.at("color").data());
		}

		void string_serialize(stl::string_map& kv_storage) const
		{
			kv_storage["color"] = std::to_string(static_cast<uint32_t>(color));
		}

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

        bool can_string_deserialize(stl::string_map& kv_storage) const
        {
            return kv_storage.contains("first_color") && kv_storage.contains("second_color");
        }
        
        void string_deserialize(stl::string_map& kv_storage)
        {
            first_color = std::stoul(kv_storage.at("first_color").data());
            second_color = std::stoul(kv_storage.at("second_color").data());
        }

		void string_serialize(stl::string_map& kv_storage) const
		{
			kv_storage["first_color"] = std::to_string(static_cast<uint32_t>(first_color));
			kv_storage["second_color"] = std::to_string(static_cast<uint32_t>(second_color));
		}

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
        resources::id_type texture_resource;

		bool can_serialize_now() const
		{
			return true;
		}

        bool can_string_deserialize(stl::string_map& kv_storage) const
        {
            return kv_storage.contains("texture_resource");
        }
        
        void string_deserialize(stl::string_map& kv_storage)
        {
            texture_resource = std::stoll(kv_storage.at("texture_resource").data());
        }
        
		void string_serialize(stl::string_map& kv_storage) const
		{
            kv_storage["texture_resource"] = std::to_string(texture_resource);
		}

		void serialize(stl::stream_vector& data) const
		{
            stl::push_memory(data, texture_resource);
		}

		void deserialize(stl::stream_vector& data)
		{
            stl::read_memory(data, texture_resource);
		}
	};

	struct scene_component
	{
        math::fvec2 scale = {};
        math::transform transform = {};

		bool can_serialize_now() const
		{
			return true;
		}
        
        bool can_string_deserialize(stl::string_map& kv_storage) const
        {
            return false;
        }
        
        void string_deserialize(stl::string_map& kv_storage)
        {
            
        }

		void string_serialize(stl::string_map& kv_storage) const
		{
            
		}

		void serialize(stl::stream_vector& data) const
		{
			stl::push_memory(data, scale);
			stl::push_memory(data, transform);
		}

		void deserialize(stl::stream_vector& data)
		{
            stl::read_memory(data, scale);
            stl::read_memory(data, transform);
		}
	};
	
	struct physics_body_component
	{
		physics::physics_body* body = nullptr;

		bool can_serialize_now() const
		{
			return body != nullptr;
		}

        bool can_string_deserialize(stl::string_map& kv_storage) const
        {
            return (kv_storage.contains("angle") &&
                    kv_storage.contains("angular_velocity") &&
                    kv_storage.contains("mass") &&
                    kv_storage.contains("mass_center_x") &&
                    kv_storage.contains("mass_center_y") &&
                    kv_storage.contains("velocity_x") &&
                    kv_storage.contains("velocity_y") &&
                    kv_storage.contains("position_x") &&
                    kv_storage.contains("position_y") &&
                    kv_storage.contains("size_x") &&
                    kv_storage.contains("size_y") &&
                    kv_storage.contains("body_type") &&
                    kv_storage.contains("material_shape") &&
                    kv_storage.contains("material_type")
            );
        }
        
		void string_deserialize(stl::string_map& kv_storage)
		{
			float angle = {};
			float angular_vel = {};
			math::fvec2 vel = {};
            math::fvec2 pos = {};
            math::fvec2 size = {};
			physics::body_type type = {};
			material::shape shape = {};
			material::type mat = {};
			float mass = {};
            math::fvec2 mass_center = {};

            ark_assert(body == nullptr, "Body is not freed yet. That means that you have a memory leak", {});
            
            angle = std::stod(kv_storage.at("angle").data());
            angular_vel = std::stod(kv_storage.at("angular_velocity").data());
            mass = std::stod(kv_storage.at("mass").data());
            mass_center[0] = std::stod(kv_storage.at("mass_center_x").data());
            mass_center[1] = std::stod(kv_storage.at("mass_center_y").data());
            vel[0] = std::stod(kv_storage.at("velocity_x").data());
            vel[1] = std::stod(kv_storage.at("velocity_y").data());
            pos[0] = std::stod(kv_storage.at("position_x").data());
            pos[1] = std::stod(kv_storage.at("position_y").data());
            size[0] = std::stod(kv_storage.at("size_x").data());
            size[1] = std::stod(kv_storage.at("size_y").data());
            
            auto& body_type_string = kv_storage.at("body_type");
            if (body_type_string == "static") {
                type = physics::body_type::static_body;
            } else if (body_type_string == "dynamic") {
                type = physics::body_type::dynamic_body;
            } else if (body_type_string == "kinematic") {
                type = physics::body_type::kinematic_body;
            }

            auto& material_shape_string = kv_storage.at("material_shape");
            if (material_shape_string == "box") {
                shape = material::shape::box;
            } else if (material_shape_string == "circle") {
                shape = material::shape::circle;
            }
            
            auto& material_type_string = kv_storage.at("material_type");
            if (material_type_string == "rubber") {
                mat = material::type::rubber;
            } else if (material_type_string == "solid") {
                mat = material::type::solid;
            }
            const physics::body_parameters parameters(angle, angular_vel, vel, pos, size, type, shape, mat, mass, mass_center);
            body = physics::schedule_creation(parameters);
		}

		void string_serialize(stl::string_map& kv_storage) const
		{
			const physics::body_parameters parameters = body->copy_parameters();
			kv_storage["velocity_x"] = std::to_string(parameters.vel.x());
			kv_storage["velocity_y"] = std::to_string(parameters.vel.y());
			kv_storage["position_x"] = std::to_string(parameters.pos.x());
			kv_storage["position_y"] = std::to_string(parameters.pos.y());
			kv_storage["size_x"] = std::to_string(parameters.size.x());
			kv_storage["size_y"] = std::to_string(parameters.size.y());

			kv_storage["angle"] = std::to_string(parameters.angle);
			kv_storage["angular_velocity"] = std::to_string(parameters.angular_vel);
			kv_storage["mass"] = std::to_string(parameters.mass);
			kv_storage["mass_center_x"] = std::to_string(parameters.mass_center.x());
			kv_storage["mass_center_y"] = std::to_string(parameters.mass_center.y());

			switch (static_cast<physics::body_type>(parameters.packed_type.type)) {
				case physics::body_type::static_body:
					kv_storage["body_type"] = "static";
					break;
				case physics::body_type::dynamic_body:
					kv_storage["body_type"] = "dynamic";
					break;
				case physics::body_type::kinematic_body:
					kv_storage["body_type"] = "kinematic";
					break;
				default:
					break;
			}

			switch (static_cast<material::shape>(parameters.packed_type.shape)) {
			case material::shape::box:
				kv_storage["material_shape"] = "box";
				break;
			case material::shape::circle:
				kv_storage["material_shape"] = "circle";
				break;
			default:
				break;
			}

			switch (static_cast<material::type>(parameters.packed_type.type)) {
			case material::type::rubber:
				kv_storage["material_type"] = "rubber";
				break;
			case material::type::solid:
				kv_storage["material_type"] = "solid";
				break;
			default:
				break;
			}
		}

		void serialize(stl::stream_vector& data) const
		{
			const physics::body_parameters parameters = body->copy_parameters();
			parameters.serialize(data);
		}

		void deserialize(stl::stream_vector& data)
		{
			ark_assert(body == nullptr, "Body is not freed yet. That means that you have a memory leak", {})
			
			const physics::body_parameters parameters(data);
			body = physics::schedule_creation(parameters);
		}
	};
	
    struct camera_component
    {
        float zoom = 0.f;
        float rotation = 0.f;
        math::transform transform;
        
        bool can_serialize_now() const
        {
            return true;
        }

        bool can_string_deserialize(stl::string_map& kv_storage) const
        {
            return false;
        }

        void string_deserialize(stl::string_map& kv_storage)
        {
        }

        void string_serialize(stl::string_map& kv_storage) const
        {
        }

        void serialize(stl::stream_vector& data) const
        {
            stl::push_memory(data, zoom);
            stl::push_memory(data, rotation);
            stl::push_memory(data, transform);
        }

        void deserialize(stl::stream_vector& data)
        {
            stl::read_memory(data, zoom);
            stl::read_memory(data, rotation);
            stl::read_memory(data, transform);
        }
    };

    struct net_linker_component
    {
        net::link_type link_id;

        bool can_serialize_now() const
        {
            return true;
        }

        bool can_string_deserialize(stl::string_map& kv_storage) const
        {
            return false;
        }

        void string_deserialize(stl::string_map& kv_storage)
        {
        	
        }

        void string_serialize(stl::string_map& kv_storage) const
        {
        }

        void serialize(stl::stream_vector& data) const
        {
            stl::push_memory(data, link_id);
        }

        void deserialize(stl::stream_vector& data)
        {
            stl::read_memory(data, link_id);
        }
    };
}
