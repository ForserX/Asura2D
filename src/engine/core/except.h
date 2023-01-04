#pragma once

namespace Asura::stl
{
	template<typename T>
	class expected
	{
		T Value;
		std::errc Except;

	public:
		expected(T Val) : Value(Val), Except((std::errc)0) {}
		expected(std::errc Val) : Except(Val) {}

		operator bool() const
		{
			return Except == (std::errc)0;
		}

		T Get() const
		{
			return Value;
		}

		T& Get()
		{
			return Value;
		}
	};
}