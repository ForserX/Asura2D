#pragma once

#ifdef _DEBUG
#define game_assert(error_code, message, exist_code) \
	if(!(error_code)) {\
		Asura::Debug::show_error(message);\
		exist_code;\
	}
#else
#define game_assert(error_code, message, exist_code) \
	if (!(error_code)) {\
		exist_code;\
	}
#endif

namespace Asura::Debug
{
	void Init();
	void Destroy();

	void show_error(stl::string_view message);
	void print_message(stl::string_view message);

	template <typename... Args>
	inline void msg(stl::string_view rt_fmt_str, Args&&... args)
	{
#ifdef OS_WINDOWS
		print_message(std::vformat(rt_fmt_str.data(), std::make_format_args(args...)));
#else
		print_message(fmt::vformat(rt_fmt_str.data(), fmt::make_format_args(args...)));
#endif
	}

	template<>
	inline void msg(stl::string_view message)
	{
		print_message(message);
	}

	void dbg_break();
	void dbg_print(stl::string_view msg);
	bool dbg_atttached();
}