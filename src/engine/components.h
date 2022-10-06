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

		bool string_deserialize(stl::string_map& kv_storage)
		{
			if (kv_storage.contains("color")) {
				color = std::stoul(kv_storage.at("color").data());
				return true;
			}

			return false;
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

		bool string_deserialize(stl::string_map& kv_storage)
		{
			bool success = false;
			if (kv_storage.contains("first_color")) {
				first_color = std::stoul(kv_storage.at("first_color").data());
				success = true;
			}

			if (kv_storage.contains("second_color")) {
				second_color = std::stoul(kv_storage.at("second_color").data());
				success = true;
			}

			return success;
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
		ImTextureID texture;

		bool can_serialize_now() const
		{
			return false;
		}

		bool string_deserialize(stl::string_map& kv_storage)
		{
			return false;
		}

		void string_serialize(stl::string_map& kv_storage) const
		{

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
		ark_float_vec2 position = {};

		bool can_serialize_now() const
		{
			return true;
		}

		bool string_deserialize(stl::string_map& kv_storage)
		{
			bool success = false;
			if (kv_storage.contains("position_x")) {
				position.x = std::stod(kv_storage.at("position_x").data());
				success = true;
			}

			if (kv_storage.contains("position_y")) {
				position.y = std::stod(kv_storage.at("position_y").data());
				success = true;
			}

			return success;
		}

		void string_serialize(stl::string_map& kv_storage) const
		{
			kv_storage["position_x"] = std::to_string(position.x);
			kv_storage["position_y"] = std::to_string(position.y);
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

		bool string_deserialize(stl::string_map& kv_storage)
		{
			bool success = false;
			float angle = {};
			float angular_vel = {};
			ark_float_vec2 vel = {};
			ark_float_vec2 pos = {};
			ark_float_vec2 size = {};
			physics::body_type type = {};
			material::shape shape = {};
			material::type mat = {};
			float mass = {};
			ark_float_vec2 mass_center = {};

			ark_assert(body == nullptr, "Body is not freed yet. That means that you have a memory leak", {})
			if (kv_storage.contains("angle")) {
				angle = std::stod(kv_storage.at("angle").data());
				success = true;
			}

			if (kv_storage.contains("angular_velocity")) {
				angular_vel = std::stod(kv_storage.at("angular_velocity").data());
				success = true;
			}

			if (kv_storage.contains("mass")) {
				mass = std::stod(kv_storage.at("mass").data());
				success = true;
			}

			if (kv_storage.contains("mass_center_x")) {
				mass_center.x = std::stod(kv_storage.at("mass_center_x").data());
				success = true;
			}

			if (kv_storage.contains("mass_center_y")) {
				mass_center.x = std::stod(kv_storage.at("mass_center_y").data());
				success = true;
			}

			if (kv_storage.contains("velocity_x")) {
				vel.x = std::stod(kv_storage.at("velocity_x").data());
			}

			if (kv_storage.contains("velocity_y")) {
				vel.y = std::stod(kv_storage.at("velocity_y").data());
			}

			if (kv_storage.contains("position_x")) {
				pos.x = std::stod(kv_storage.at("position_x").data());
			}

			if (kv_storage.contains("position_y")) {
				pos.y = std::stod(kv_storage.at("position_y").data());
			}

			if (kv_storage.contains("size_x")) {
				size.x = std::stod(kv_storage.at("size_x").data());
			}

			if (kv_storage.contains("size_y")) {
				size.y = std::stod(kv_storage.at("size_y").data());
			}

			if (kv_storage.contains("body_type")) {
				auto& body_type_string = kv_storage.at("body_type");
				if (body_type_string == "static") {
					type = physics::body_type::static_body;
				} else if (body_type_string == "dynamic") {
					type = physics::body_type::dynamic_body;
				} else if (body_type_string == "kinematic") {
					type = physics::body_type::kinematic_body;
				}
			}

			if (kv_storage.contains("material_shape")) {
				auto& body_type_string = kv_storage.at("material_shape");
				if (body_type_string == "box") {
					shape = material::shape::box;
				} else if (body_type_string == "circle") {
					shape = material::shape::circle;
				}
			}

			if (kv_storage.contains("material_type")) {
				auto& body_type_string = kv_storage.at("material_type");
				if (body_type_string == "rubber") {
					mat = material::type::rubber;
				} else if (body_type_string == "solid") {
					mat = material::type::solid;
				}
			}

			if (success) {
				const physics::body_parameters parameters(angle, angular_vel, vel, pos, size, type, shape, mat, mass, mass_center);
				body = physics::schedule_creation(parameters);
			}

			return success;
		}

		void string_serialize(stl::string_map& kv_storage) const
		{
			const physics::body_parameters parameters = body->copy_parameters();
			kv_storage["velocity_x"] = std::to_string(parameters.vel.x);
			kv_storage["velocity_y"] = std::to_string(parameters.vel.y);
			kv_storage["position_x"] = std::to_string(parameters.pos.x);
			kv_storage["position_y"] = std::to_string(parameters.pos.y);
			kv_storage["size_x"] = std::to_string(parameters.size.x);
			kv_storage["size_y"] = std::to_string(parameters.size.y);

			kv_storage["angle"] = std::to_string(parameters.angle);
			kv_storage["angular_velocity"] = std::to_string(parameters.angular_vel);
			kv_storage["mass"] = std::to_string(parameters.mass);
			kv_storage["mass_center_x"] = std::to_string(parameters.mass_center.x);
			kv_storage["mass_center_y"] = std::to_string(parameters.mass_center.y);

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
	
	struct dynamic_visual_component
	{
		stl::vector<ark_float_vec2> points;
		
		bool can_serialize_now() const
		{
			return !points.empty();
		}

		bool string_deserialize(stl::string_map& kv_storage)
		{
			return false;
		}

		void string_serialize(stl::string_map& kv_storage) const
		{
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

#define DECLARE_SERIALIZABLE_FLAGS \
	entities::background_flag, \
	entities::drawable_flag, \
	entities::ground_flag, \
	entities::level_flag, \
	entities::net_id_flag, \
	entities::net_controlled_flag 

#define DECLARE_SERIALIZABLE_TYPES \
	DECLARE_SERIALIZABLE_FLAGS, \
	entities::draw_color_component, \
	entities::draw_gradient_component, \
	entities::draw_texture_component, \
	entities::scene_component, \
	entities::physics_body_component, \
	entities::dynamic_visual_component

#define DECLARE_SERIALIZABLE_ENTITY_TYPES \
	DECLARE_SERIALIZABLE_FLAGS, \
	DECLARE_SERIALIZABLE_TYPES

#define DECLARE_NON_SERIALIZABLE_TYPES \
	entities::garbage_flag, \
	entities::non_serializable_flag, \
	entities::dont_free_after_reset_flag 

#define DECLARE_ENTITIES_TYPES \
	DECLARE_SERIALIZABLE_TYPES, \
	DECLARE_NON_SERIALIZABLE_TYPES

}