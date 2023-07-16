#pragma once

namespace Asura::UI
{
	class Console;
}

namespace Asura::Console
{
	class CommandTemplate
	{
		friend class Asura::UI::Console;

	public:
		using CallbackType = void();

	protected:
		stl::string Hint = "";
		CallbackType* Callback = nullptr;

	public:
		CommandTemplate(stl::string Name);
		virtual ~CommandTemplate() = default;

		virtual void SetupHint() = 0;
		virtual void Exec(stl::string_view Command) = 0;
	};

	class CommandBoolean : public CommandTemplate
	{
		bool* Value;

	public:
		inline CommandBoolean(stl::string Name, bool* Val) :
			CommandTemplate(Name), Value(Val)
		{}

		inline CommandBoolean(stl::string Name, bool* Val, CallbackType* InCallback) :
			CommandTemplate(Name), Value(Val)
		{
			Callback = InCallback;
		}

		virtual void SetupHint() override
		{
			Hint = "true/false";
		}

		virtual void Exec(stl::string_view Command) override
		{
			if (Command == "true" || Command == "1")
			{
				*Value = true;
			}
			else
			{
				*Value = false;
			}

			if (Callback != nullptr)
			{
				Callback();
			}
		}
	};

	template <typename T>
	class CommandNumber : public CommandTemplate
	{
		T* Value;
		T Min;
		T Max;

	public:
		CommandNumber(stl::string Name, T* Val, T NewMin, T NewMax) :
			CommandTemplate(Name), Min(NewMin), Max(NewMax), Value(Val)
		{}

		CommandNumber(stl::string Name, T* Val, CallbackType* InCallback) :
			CommandTemplate(Name), Min(0), Max(1000), Value(Val)
		{
			Callback = InCallback;
		}

		virtual void SetupHint() override
		{
			Hint = stl::to_string(Min) + " - " + stl::to_string(Max);
		}

		virtual void Exec(stl::string_view Command) override
		{
			if constexpr (std::is_floating_point_v<T>)
			{
				(*Value) = (T)atof(Command.data());
			}
			else if constexpr (std::is_signed_v<T>)
			{
				(*Value) = (T)atoll(Command.data());
			}
			else
			{
				(*Value) = (T)stl::stoull(Command);
			}

			if (Callback != nullptr)
			{
				Callback();
			}
		}
	};

	class CommandCallback : public CommandTemplate
	{
	public:
		CommandCallback(stl::string Name, CallbackType* InCallback) :
			CommandTemplate(Name)
		{
			Callback = InCallback;
		}

		virtual void SetupHint() override {}
		virtual void Exec(stl::string_view Command) override
		{
			if (Callback != nullptr)
			{
				Callback();
			}
		}
	};

	template <class T>
	void MakeConsoleCommand(const char* Name)
	{
		T* Cmd = new T(Name);
		Cmd->SetupHint();
	}

	template <class T, typename V>
	void MakeConsoleCommand(const char* Name, V* Value)
	{
		T* Cmd = new T(Name, Value);
		Cmd->SetupHint();
	}

	template <class T, typename V>
	void MakeConsoleCommand(const char* Name, V* Value, auto Min, auto Max)
	{
		T* Cmd = new T(Name, Value, (V)Min, (V)Max);
		Cmd->SetupHint();
	}

	template <class T, typename V>
	void MakeConsoleCommand(const char* Name, V* Value, auto Callback)
	{
		T* Cmd = new T(Name, Value, Callback);
		Cmd->SetupHint();
	}

	template <class T, typename V>
	void MakeConsoleCommand(const char* Name, std::nullptr_t, V Callback)
	{
		T* Cmd = new T(Name, Callback);
		Cmd->SetupHint();
	}
}