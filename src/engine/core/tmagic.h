#pragma once

namespace ark::stl
{
	template <class T> 
	using clear_type = std::remove_cv_t<std::remove_reference_t<T>>;

	namespace meta
	{
		namespace internal
		{
			template <class Default, class AlwaysVoid, template<class...> class Op, class... Args>
			struct detector
			{
				using value_t = std::false_type;
				using type = Default;
			};

			template <class Default, template<class...> class Op, class... Args>
			struct detector<Default, std::void_t<Op<Args...>>, Op, Args...>
			{
				using value_t = std::true_type;
				using type = Op<Args...>;
			};

			struct nonesuch {};
		}

		template <template<class...> class Op, class... Args>
		using is_detected = typename internal::detector<internal::nonesuch, void, Op, Args...>::value_t;

		template <template<class...> class Op, class... Args>
		using detected_t = typename internal::detector<internal::nonesuch, void, Op, Args...>::type;
	}

	template <typename T>
	using detect_flag = decltype(T::flag);

	template <typename T>
	using detect_custom_serialize = decltype(T::custom_serialize);

	template <typename T>
	using detect_string_serialize = decltype(T::string_serialize);

	template <typename T>
	constexpr bool is_flag_v = stl::meta::is_detected<detect_flag, T>::value;

	template <typename T>
	constexpr bool is_custom_serialize_v = stl::meta::is_detected<detect_custom_serialize, T>::value;

	template <typename T>
	constexpr bool is_string_serialize_v = stl::meta::is_detected<detect_string_serialize, T>::value;

	template<typename T>
	void hash_combine(std::int64_t& s, const T& v)
	{
		std::hash<T> h;
		s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
	}

	template<typename T>
	struct function_equal
	{
		constexpr bool operator()(const T& left, const T& right) const
		{
			return left.get_id() == right.get_id();
		}
	};

	template<typename T>
	struct function_hasher
	{
		std::int64_t operator()(const T& s) const
		{
			std::int64_t res = 0;
			hash_combine(res, s.get_id());
			return res;
		}
	};
}