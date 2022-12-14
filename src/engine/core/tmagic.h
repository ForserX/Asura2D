#pragma once

namespace Asura::stl
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

		template<typename T, template<typename...> class Ref>
		struct is_specialization : std::false_type {};

		template<template<typename...> class Ref, typename... Args>
		struct is_specialization<Ref<Args...>, Ref> : std::true_type {};
	}

	template <typename T>
	using detect_flag = decltype(T::flag);

	template <typename T>
	using detect_custom_serialize = decltype(T::custom_serialize);

	template <typename T>
	using detect_string_serialize = decltype(T::string_serialize);

	template <typename T>
	constexpr bool contains_flag_v = stl::meta::is_detected<detect_flag, T>::value;

	template <typename T>
	constexpr bool is_custom_serialize_v = stl::meta::is_detected<detect_custom_serialize, T>::value;

	template <typename T>
	constexpr bool is_string_serialize_v = stl::meta::is_detected<detect_string_serialize, T>::value;
}