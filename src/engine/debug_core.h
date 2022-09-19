#pragma once
#include <string>
#include <format>

#ifdef _DEBUG
#define ark_assert(error_code, message, exist_code) \
	if(!error_code) {\
		ark::debug::show_error(message);\
	}\
	exist_code
#else
#define assert(error_code, message, exist_code)
#endif

namespace ark::debug
{
	void show_error(std::string_view message);

	class logger
	{
		logger() = default;
		~logger() = default;

		logger(const logger&) = delete;
		virtual logger& operator=(const logger&) = delete;
		virtual logger&& operator=(const logger&&) = delete;

	public:
		void msg(std::string_view format, std::format_args args);
	};
}