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
			return stl::to_string(static_cast<uint32_t>(value));
		} else if constexpr (std::is_same_v<U, bool>) {
			return stl::string(value == true ? "true" : "false");
		} else if constexpr (std::is_enum_v<U>) {
			return stl::to_string(static_cast<int64_t>(value));
		} else {
			return stl::to_string(value);
		}
	}

	template<typename T>
	T unstringify(const stl::string& value)
	{
		auto stringify_type = []<typename U>(U ival)
		{
			if constexpr (stl::is_string_serialize_v<U>) {
				return ival.to_string();
			} else if constexpr (std::is_same_v<U, ImColor>) {
				return std::to_string(static_cast<uint32_t>(ival));
			} else if constexpr (std::is_same_v<U, bool>) {
				return stl::string(ival == true ? "true" : "false");
			} else if constexpr (std::is_integral_v<U>) {
				return std::to_string(ival);
			} else if constexpr (std::is_enum_v<U>) {
				return std::to_string(static_cast<int64_t>(ival));
			} else {
				return std::to_string(ival);
			}
		};

		using ClearType = stl::clear_type<T>;
		if constexpr (stl::meta::is_specialization<ClearType, stl::vector>::value) {
			stl::string combined_string = "{ ";
			for (const auto& elem : value) {
				combined_string += stringify_type(elem);
				combined_string += " ";
			}

			return combined_string;
		} else {
			return stringify_type(value);
		}
	}

	template<typename T, typename U = stl::clear_type<T>>
	U unstringify(const stl::string& value)
	{
		auto unstrigify_type = [](auto& out, const stl::string& string_value)
		{
			if constexpr (std::is_floating_point_v<U>) {
				out = static_cast<U>(std::stod(string_value));
			} else if constexpr (std::is_same_v<U, ImColor>) {
				out = U(std::stoul(string_value));
			} else if constexpr (stl::is_string_serialize_v<U>) {
				out = U::unstrigify(string_value);
			} else if constexpr (std::is_same_v<U, bool>) {
				out = !string_value.compare("true");
			} else if constexpr (std::is_integral_v<U>) {
				if constexpr (std::is_unsigned_v<U>) {
					out = static_cast<U>(std::stoull(string_value));
				} else {
					out = static_cast<U>(std::stoll(string_value));
				}
			} else if constexpr (std::is_enum_v<U>) {
				out = static_cast<U>(std::stoll(string_value));
			} else {
				out = string_value.data();
			}
		};

		using ClearType = stl::clear_type<T>;
		if constexpr (stl::meta::is_specialization<ClearType, stl::vector>::value) {
			stl::vector<ClearType::value_type> OutValue;

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

			const size_t offset_end = value.find_first_of('}');
			while (offset != size_t(-1) && offset < offset_end) {
				const size_t begin_offset = value.find_first_not_of(' ', offset);
				const size_t end_offset = value.find_first_of(' ', begin_offset);
				offset = end_offset;

				typename ClearType::value_type out_value;
				unstrigify_type(out_value, stl::string(value.begin() + begin_offset, value.begin() + end_offset));
				OutValue.push_back(unstrigify_type(stl::string(value.begin() + begin_offset, value.begin() + end_offset)));
			}

			return OutValue;
		} else {
			U out_value = {};
			unstrigify_type(out_value, value);
			return out_value;
		}
	}

	/*
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
	*/

	template<typename T>
	constexpr stl::string_view get_type_string()
	{
		if constexpr (stl::meta::is_specialization<T, stl::vector>::value) {
			const static stl::string type_string = stl::string("vec_") + stl::get_type_string<T::value_type>();
			return type_string.data();
		} else if constexpr (std::is_floating_point_v<T>) {
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
	stl::string combine_string(const char* var_name)
	{
		return stl::string(get_type_string<T>().data()).append(var_name);
	}
}
