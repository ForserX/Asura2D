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

	template<typename>
	struct is_stl_vector : std::false_type {};

	template<typename T>
	struct is_stl_vector<stl::vector<T>> : std::true_type {};

	template<typename T>
	stl::string stringify(T value)
	{
		using U = stl::clear_type<T>;

		auto stringify_type = []<typename CT>(CT value) -> stl::string
		{
			if constexpr (std::is_same_v<CT, math::transform>) {
				return value.to_string();
			} else if constexpr (std::is_same_v<CT, math::fvec2>) {
				return value.to_string();
			} else if constexpr (std::is_same_v<U, ImColor>) {
				return stl::to_string(static_cast<uint32_t>(value));
			} else if constexpr (std::is_same_v<U, bool>) {
				return stl::string(value == true ? "true" : "false");
			} else if constexpr (std::is_enum_v<U>) {
				return stl::to_string(static_cast<int64_t>(value));
			} else if constexpr (std::is_integral_v<U>) {
				return stl::to_string(static_cast<int64_t>(value));
			} else {
				return stl::to_string(value);
			}
		};

		if constexpr (is_stl_vector<U>::value) {
			using vec_elem_type = typename U::value_type;

			stl::string vector_string;
			vector_string += "{ ";
			for (const vec_elem_type& elem : value) {
				vector_string += stringify_type(elem);
				vector_string += " ";
			}

			vector_string += "}";
			return vector_string;
		} else {
			return stringify_type(value);
		}
	}

	template<typename T, typename U = stl::clear_type<T>>
	U unstringify(const stl::string_view& value)
	{
		auto unstrigify_type = []<typename CT>(CT& val, const stl::string_view& string_value)
		{
			if constexpr (std::is_same_v<CT, math::vec2<float>>) {
				val.from_string(string_value);
			} else if constexpr (std::is_floating_point_v<CT>) {
				val = static_cast<CT>(stl::stod(string_value));
			} else if constexpr (std::is_same_v<CT, ImColor>) {
				val = CT(stl::stoul(string_value));
			} else if constexpr (stl::is_string_serialize_v<CT>) {
				val = CT::unstrigify(string_value);
			} else if constexpr (std::is_same_v<CT, bool>) {
				val = !string_value.compare("true");
			} else if constexpr (std::is_integral_v<CT>) {
				if constexpr (std::is_unsigned_v<CT>) {
					val = static_cast<CT>(stl::stoull(string_value));
				} else {
					val = static_cast<CT>(stl::stoll(string_value));
				}
			} else if constexpr (std::is_enum_v<CT>) {
				val = static_cast<CT>(stl::stoll(string_value));
			} else {
				val = string_value.data();
			}
		};

		if constexpr (is_stl_vector<U>::value) {
			size_t offset = value.find_first_of('{');
			if (offset == size_t(-1)) {
				// #TODO: parsing error
				return {};
			}

			offset = value.find_first_not_of(' ', offset + 1);
			if (offset == size_t(-1)) {
				// #TODO: parsing error
				return {};
			}

			using vec_elem_type = typename U::value_type;
			stl::vector<vec_elem_type> values_vector;

			const size_t offset_end = value.find_first_of('}');
			while (offset != size_t(-1) && offset < offset_end) {
				const size_t begin_offset = value.find_first_not_of(' ', offset);
				const size_t end_offset = value.find_first_of(' ', begin_offset);
				offset = end_offset;

				vec_elem_type temp_value;
				unstrigify_type(temp_value, stl::string(value.data() + begin_offset, value.data() + end_offset));
				values_vector.emplace_back(std::move(temp_value));
			}

			return values_vector;
		} else {
			U ret_val;
			unstrigify_type(ret_val, value);
			return ret_val;
		}
	}

	template<typename T>
	constexpr stl::string_view get_type_string()
	{
		if constexpr (is_stl_vector<T>::value) {
			return "arr_";
		} else if constexpr (std::is_same_v<T, math::vec2<float>>) {
			return "vecf_";
		} else if constexpr (std::is_floating_point_v<T>) {
			return "f_";
		} else if constexpr (std::is_same_v<T, ImColor>) {
			return "c_";
		} else if constexpr (std::is_same_v<T, math::transform>) {
			return "t_"; 
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
	stl::string combine_string(const char* var_name)
	{
		return stl::string(get_type_string<T>().data()).append(var_name);
	}
}
