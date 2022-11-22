#pragma once

#ifdef _DEBUG
#define ark_assert(error_code, message, exist_code) \
	if(!(error_code)) {\
		ark::debug::show_error(message);\
		exist_code;\
	}
#else
#define ark_assert(error_code, message, exist_code) \
	if (!(error_code)) {\
		exist_code;\
	}
#endif

namespace ark::debug
{
	void init();
	void destroy();

	void show_error(stl::string_view message);
	void print_message(stl::string_view message);

	template <typename... Args>
	inline void msg(stl::string_view rt_fmt_str, Args&&... args)
	{
#ifdef __linux__
		print_message(fmt::vformat(rt_fmt_str.data(), fmt::make_format_args(args...)));
#else
		print_message(std::vformat(rt_fmt_str.data(), std::make_format_args(args...)));
#endif
	}

	template<>
	inline void msg(stl::string_view message)
	{
		print_message(message);
	}
}
