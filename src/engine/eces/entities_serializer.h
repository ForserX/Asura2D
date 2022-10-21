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
