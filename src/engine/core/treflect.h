#pragma once

namespace ark::stl
{
	template<typename T>
	void write_memory(stream_vector& data, T& value)
	{
		char* ptr = data.second.data();
		std::memcpy(reinterpret_cast<void*>(&ptr[data.first]), reinterpret_cast<const void*>(&value), sizeof(value));
		data.first += sizeof(value);
	}

	template<typename T>
	void read_memory(stream_vector& data, T& value)
	{
		const char* ptr = data.second.data();
		std::memcpy(reinterpret_cast<char*>(&value), &ptr[data.first], sizeof(T));
		data.first +=  sizeof(value);
	}

	template<typename T>
	void push_memory(stream_vector& data, T& value)
	{
		using U = stl::clear_type<T>;

		const size_t idx = data.second.size();
		data.second.resize(data.second.size() + sizeof(T));

		char* ptr = data.second.data();
		ptr = &ptr[idx];
		U* data_ptr = reinterpret_cast<U*>(ptr);
		*data_ptr = value;
	}
}