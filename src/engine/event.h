﻿#pragma once

namespace ark::event
{
	using parameter = std::variant<
 		std::monostate,
 		std::string_view,
 		entity_view,
 		int64_t,
 		ark_int_vec2,
 		ark_float_vec2,
 		double,
 		bool
	>;

	namespace internal
	{
		// This is base type for callback, which was created for this reasons:
		// 1. increase SFO (Small Function Optimization) buffer to improve performance on parameters copying.
		// 2. disable exceptions (because we don't use them)
		// 3. pass "ArgsCount" to increase size of SFO buffer
		template<typename Arg, std::size_t ArgsCount>
		using base_callback = fu2::function_base<
			true,
			true,
			fu2::capacity_fixed<(sizeof(parameter) * ArgsCount) + sizeof(void*) * 3 /* this + func + parameter capture */>,
			false,
			false,
			Arg
		>;
		
		// Basically, this template is fucking awful, because it was created only for one purpose - obtaining the type
		// of function arguments. We need to get count of function arguments manually to return delegate with
		// acceptable type for it.
		template<typename> struct function_args final {};
		template<typename T, typename... Args>
		struct function_args<T(Args...)> final
		{
			static constexpr std::size_t args_count = sizeof...(Args);
		};

		using callback_0_arg = void() const;
		using callback_1_arg = void(parameter) const;
		using callback_2_arg = void(parameter, parameter) const;
		
		using callback_0 = base_callback<callback_0_arg, 0>;
		using callback_1 = base_callback<callback_1_arg, 1>;
		using callback_2 = base_callback<callback_2_arg, 2>;
		constexpr int64_t max_parameters_count = 2;
		
		// Yes, this is variant in variant. Because I don't want to create separate functions.
		using callback = std::variant<callback_0, callback_1, callback_2>;

		struct callback_equal
		{
			using callback_type = callback;
			
			constexpr bool operator()(const callback_type& left, const callback_type& right) const
			{
				bool compare_value = false;
				std::visit([&]<typename T0>(T0&& arg_0) {
					std::visit([&]<typename T1>(T1&& arg_1) {
						compare_value = (arg_0.get_id() == arg_1.get_id());
					}, right);
				}, left);

				return compare_value;
			}
		};

		struct callback_hasher
		{
			using callback_type = callback;
			
			std::int64_t operator()(const callback_type& s) const
			{
				std::int64_t res = 0;

				std::visit([&]<typename T0>(T0&& arg_0) {
					stl_shit::hash_combine(res, arg_0.get_id());
				}, s);

				return res;
			}
		};
		
		using subscribers_storage = std::unordered_set<
			callback,
			callback_hasher,
			callback_equal
		>;
		
		using event_callback_storage = std::unordered_map<
			std::string_view,
			subscribers_storage
		>;
	}

	void init();
	void destroy();
	void tick();

	void create(std::string_view name, int64_t parameters_count);
	void remove(std::string_view name);

	bool exists(std::string_view name);

	namespace internal
	{
		// Another template magic, that was created for lambdas and captures. I hate this.
		template<std::size_t args_count>
		auto make_callback(auto&& functor)
		{
			static_assert(
				args_count <= max_parameters_count && args_count != -1,
				"Invalid parameters count."
			);
			
			if constexpr (args_count == 0) {
				auto return_callback = callback_0(functor);
				return callback(return_callback);
			} else if constexpr (args_count == 1) {
				auto return_callback = callback_1(functor);
				return callback(return_callback);
			} else if constexpr (args_count == 2) {
				auto return_callback = callback_2(functor);
				return callback(return_callback);
			}

			return callback();
		}
		
		void subscribe(std::string_view name, const callback& sub_callback);
		void unsubscribe(std::string_view name, const callback& sub_callback);

		void trigger(std::string_view name, parameter callback_parameter_1 = std::monostate(), parameter callback_parameter_2 = std::monostate());
	}
		
	template<std::size_t args_count>
	auto subscribe(std::string_view name, auto&& functor)
	{
		auto callback_delegate = internal::make_callback<args_count>(functor);
		internal::subscribe(name, callback_delegate);
		return callback_delegate;
	}
	
	template<class... Args>
	void trigger(std::string_view name, Args... arguments)
	{
		constexpr size_t args_count = sizeof...(arguments);
		internal::trigger(name, arguments...);
	}
}
