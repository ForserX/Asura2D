#pragma once
#include <box2d/box2d.h>

namespace Asura
{
	class CollisionLister;
}

namespace Asura::Physics
{
	// 4 bits
	enum class BodyType : uint8_t
	{
		invalid = 0,

		Static,
		Dynamic,
		Kinematic,
		Preset
	};

	inline b2BodyType Asura2Box2DBodyType(uint8_t type)
	{
		switch (static_cast<BodyType>(type))
		{
		case BodyType::Kinematic:
			return b2_kinematicBody;
			break;
		case BodyType::Static:
			return b2_staticBody;
			break;
		case BodyType::Dynamic:
			return b2_dynamicBody;
			break;
		default:
			game_assert(false, "Invalid body type", {});
			break;
		}

		return b2_dynamicBody;
	}

	inline BodyType Box2D2AsuraBodyType(b2BodyType type)
	{
		switch (type) 
		{
		case b2_kinematicBody:
			return BodyType::Kinematic;
		case b2_staticBody:
			return BodyType::Static;
		case b2_dynamicBody:
			return BodyType::Dynamic;
		default:
			game_assert(false, "Invalid body type", {});
			break;
		}

		return BodyType::Dynamic;
	}

	struct body_parameters
	{
		struct packed
		{
			packed(uint8_t in_type, uint8_t in_shape, uint8_t in_mat)
				: type(in_type), shape(in_shape), mat(in_mat) {}

			packed() : type(0), shape(0), mat(0) {}

			uint8_t type : 4;
			uint8_t shape : 4;
			uint8_t mat;
		} packed_type;

		float mass = 0.f;
		float angle = 0.f;
		float angular_vel = 0.f;
		Math::FVec2 vel;
		Math::FVec2 pos;
		Math::FVec2 size;
		Math::FVec2 mass_center;

		body_parameters() = default;
		body_parameters(
			float in_angle,
			float in_vel_angle,
			Math::FVec2 in_vel,
			Math::FVec2 in_pos,
			Math::FVec2 in_size,
			BodyType in_type = BodyType::Dynamic,
			Material::shape in_shape = Material::shape::box,
			Material::type in_mat = Material::type::solid,
			float in_mass = 0.f,
			Math::FVec2 in_mass_center = {}
		) :
			angle(in_angle), angular_vel(in_vel_angle),
			vel(in_vel), pos(in_pos), size(in_size), mass(in_mass),
			packed_type(static_cast<uint8_t>(in_type), static_cast<uint8_t>(in_shape), static_cast<uint8_t>(in_mat))
		{
			if (in_mass_center.empty())
			{
				mass_center = { size.x / 2, size.y / 2 };
			}
			else {
				mass_center = in_mass_center;
			}
		}

		body_parameters(stl::stream_vector& data)
		{
			deserialize(data);
		}

		body_parameters(stl::string_map& data)
		{
			string_deserialize(data);
		}

		operator b2BodyDef() const
		{
			b2BodyDef body_def = {};
			body_def.angle = angle;
			body_def.angularVelocity = angular_vel;
			body_def.linearVelocity = b2Vec2(vel);
			body_def.position = b2Vec2(pos);
			body_def.type = Asura2Box2DBodyType(packed_type.type);
			return body_def;
		}

		operator b2MassData() const
		{
			b2MassData mass_data = {};
			mass_data.center = b2Vec2(mass_center);
			mass_data.mass = mass;
			return mass_data;
		}

		void serialize(stl::stream_vector& data) const
		{
			stl::push_memory(data, angle);
			stl::push_memory(data, angular_vel);
			stl::push_memory(data, mass);
			stl::push_memory(data, mass_center);
			stl::push_memory(data, vel);
			stl::push_memory(data, pos);
			stl::push_memory(data, size);
			stl::push_memory(data, packed_type);
		}

		void deserialize(stl::stream_vector& data)
		{
			stl::read_memory(data, angle);
			stl::read_memory(data, angular_vel);
			stl::read_memory(data, mass);
			stl::read_memory(data, mass_center);
			stl::read_memory(data, vel);
			stl::read_memory(data, pos);
			stl::read_memory(data, size);
			stl::read_memory(data, packed_type);
		}

		void string_serialize(stl::string_map& data) const
		{
			data["f_angle"] = stl::stringify(angle);
			data["f_angular_vel"] = stl::stringify(angular_vel);
			data["f_mass"] = stl::stringify(mass);

			data["vecf_mass_center"] = stl::stringify(mass_center);
			data["vecf_vel"] = stl::stringify(vel);
			data["vecf_pos"] = stl::stringify(pos);
			data["vecf_size"] = stl::stringify(size);

			uint32_t temp_value = packed_type.mat;
			data["i_mat"] = stl::stringify(temp_value);

			temp_value = packed_type.shape;
			data["i_shape"] = stl::stringify(temp_value);

			temp_value = packed_type.type;
			data["i_type"] = stl::stringify(temp_value);
		}

		void string_deserialize(stl::string_map& data)
		{
			auto read_on_existing = [&data]<typename T>(const char* name, T & out_value)
			{
				if (data.contains(name)) {
					out_value = stl::unstringify<T>(data[name]);
				}
			};

			read_on_existing("f_angle", angle);
			read_on_existing("f_angular_vel", angular_vel);
			read_on_existing("f_mass", mass);

			read_on_existing("vecf_mass_center", mass_center);
			read_on_existing("vecf_vel", vel);
			read_on_existing("vecf_pos", pos);
			read_on_existing("vecf_size", size);

			if (data.contains("i_mat"))
			{
				packed_type.mat = stl::unstringify<uint8_t>(data["i_mat"]);
			}

			if (data.contains("i_shape"))
			{
				packed_type.mat = stl::unstringify<uint8_t>(data["i_shape"]);
			}

			if (data.contains("i_type"))
			{
				packed_type.mat = stl::unstringify<uint8_t>(data["i_type"]);
			}

		}
	};
	
	class PhysicsBody
	{
	private:
		bool created = false;
		bool destroyed = false;
		bool garbage_destroyed = false;
		b2Body* body = nullptr;
		body_parameters parameters;

	public:
		PhysicsBody() = delete;
		PhysicsBody(const PhysicsBody&) = delete;

		PhysicsBody(PhysicsBody&& OldData) noexcept : body(OldData.body) {};
		PhysicsBody(b2Body* copy_body) : body(copy_body) {};
		PhysicsBody(body_parameters in_parameters);

		~PhysicsBody();

		inline bool IsCreated() const { return created; }
		inline bool IsGarbage() const { return garbage_destroyed; }
		inline bool IsDestroyed() const { return destroyed; }
		inline bool is_enabled() const { return (body ? body->IsEnabled() : false); }
		
		b2Body* get_body() const { return body; }
		const body_parameters& get_parameters() const { return parameters; }

		void SetAsGarbage();

	public:
		BodyType GetType() const;
		
		float get_mass() const;
		float get_angle() const;
		float get_angular_velocity() const;

		Math::FVec2 get_mass_center() const;
		Math::FVec2 get_velocity() const;
		Math::FVec2 get_position() const;

		Math::FRect get_rect() const;

		bool operator==(const PhysicsBody& LeftBody) const
		{
			return this->body == LeftBody.body;
		}
	public:
		void set_body_type(BodyType new_type);
		void set_mass(float new_mass);
		void set_mass_center(const Math::FVec2& new_center);
		void set_angle(float new_angle);
		void set_angular_velocity(float new_angular_vel);
		void set_velocity(const Math::FVec2& new_vel);
		void set_position(const Math::FVec2& new_pos);

		bool IsFlying() const;

		void BlockRotation(bool Value);

		void ApplyImpulse(const Math::FVec2& impulse);
		void apply_angular_impulse(float impulse);

	public:
		body_parameters copy_parameters() const;

	public:
		void Create();
		void Destroy();
	};
}