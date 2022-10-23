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
		std::memcpy(reinterpret_cast<char*>(&value), &ptr[data.first], sizeof(stl::clear_type<T>));
		data.first +=  sizeof(value);
	}

	template<typename T>
	void push_memory(stream_vector& data, T& value)
	{
		const size_t idx = data.second.size();
		data.second.resize(data.second.size() + sizeof(stl::clear_type<T>));

		char* ptr = data.second.data();
		std::memcpy(reinterpret_cast<void*>(&ptr[idx]), reinterpret_cast<const void*>(&value), sizeof(value));
		data.first += sizeof(value);
	}

	template<typename T>
	stl::string stringify(T value)
	{
		using U = stl::clear_type<T>;
		if constexpr (std::is_same_v<U, math::transform>) {
			return value.to_string();
		} else if constexpr (std::is_same_v<U, ImColor>) {
			return std::to_string(static_cast<uint32_t>(value));
		} else if constexpr (std::is_same_v<U, bool>) {
			return stl::string(value == true ? "true" : "false");
		} else if constexpr (std::is_enum_v<U>) {
			return std::to_string(static_cast<int64_t>(value));
		} else {
			return std::to_string(value);
		}
	}

	template<typename T>
	T unstringify(const stl::string& value)
	{
		using U = stl::clear_type<T>;
		if constexpr (std::is_floating_point_v<U>) {
			return static_cast<U>(std::stod(value));
		} else if constexpr (std::is_same_v<U, ImColor>) {
			return T(std::stoul(value));
		} else if constexpr (stl::is_string_serialize_v<U>) {
			return U::from_string(value);
		} else if constexpr (std::is_same_v<U, bool>) {
			return value == "true";
		} else if constexpr (std::is_integral_v<U>) {
			if constexpr (std::is_unsigned_v<U>) {
				return static_cast<U>(std::stoull(value));
			} else {
				return static_cast<U>(std::stoll(value));
			}
		} else if constexpr (std::is_enum_v<U>) {
			return static_cast<U>(std::stoll(value));
		} else {
			return value.data();
		}
	}

	void inspect(const stl::string& skey, const stl::string& sval, auto&& func)
	{
		if (!skey.compare("b_")) {
			func(stl::unstringify<bool>(sval));
		} else if (skey.compare("i_")) {
			func(stl::unstringify<int64_t>(sval));
		} else if (skey.compare("c_")) {
			func(stl::unstringify<ImColor>(sval));
		} else if (skey.compare("f_")) {
			func(stl::unstringify<double>(sval));
		} else if (skey.compare("t_")) {
			func(stl::unstringify<math::transform>(sval));
		} else if (skey.compare("v_")) {
			func(stl::unstringify<math::fvec2>(sval));
		} else if (skey.compare("s_")) {
			func(stl::unstringify<stl::string_view>(sval));
		} else {
			func(stl::unstringify<stl::string_view>(sval));
		}
	}

	template<typename T>
	constexpr stl::string_view get_type_string()
	{
		if constexpr (std::is_floating_point_v<T>) {
			return "f_";
		} else if constexpr (std::is_same_v<T, ImColor>) {
			return "c_";
		} else if constexpr (std::is_same_v<T, math::transform>) {
			return "f_"; 
		} else if constexpr (std::is_same_v<T, math::fvec2>) {
			return "v_";
		} else if constexpr (std::is_same_v<T, bool>) {
			return "b_";
		} else if constexpr (std::is_integral_v<T>) {
			return "i_";
		} else if constexpr (std::is_enum_v<T>) {
			return "i_";
		}
		
		return "s_";	
	}

	template<typename T>
	std::string combine_string(const char* var_name)
	{
		return std::string(get_type_string<T>().data()).append(var_name);
	}
}
