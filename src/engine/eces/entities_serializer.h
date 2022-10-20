#pragma once

namespace ark::entities
{
	const std::chrono::nanoseconds& get_last_serialize_time();

	void deserialize_state(stl::string_view state_name);
	void serialize_state(stl::string_view state_name);

	void string_serialize(stl::tree_string_map& data);
	void string_deserialize(const stl::tree_string_map& data);

	void serialize(stl::stream_vector& data);
	void deserialize(stl::stream_vector& data);
}
