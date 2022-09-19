#pragma once

#ifdef _DEBUG
#define ark_assert(error_code, message, exist_code) \
	if(!(error_code)) {\
		ark::debug::show_error(message);\
		exist_code;\
	}
#else
#define assert(error_code, message, exist_code)
#endif

namespace ark::debug
{
	void init();
	void destroy();

	void show_error(std::string_view message);
	void print_message(std::string_view message);

	template <typename... Args>
	inline void msg(std::string_view rt_fmt_str, Args&&... args)
	{
		print_message(std::vformat(rt_fmt_str, std::make_format_args(args...)));
	}

	template<>
	inline void msg(std::string_view message)
	{
		print_message(message);
	}
}