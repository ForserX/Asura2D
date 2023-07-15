#pragma once

#include "console_tmagic.h"

namespace Asura::UI
{
	class Console
	{
		friend class Asura::Console::CommandTemplate;

	protected:
		char InputBuf[256] = {};
		ImVector<char*> Items;
		ImVector<char*> History;

		// -1: new line, 0..History.Size-1 browsing history.
		int HistoryPos;

		ImGuiTextFilter Filter;
		bool AutoScroll;
		bool ScrollToBottom;

		stl::hash_map<stl::string, Asura::Console::CommandTemplate*> CmdList;

	public:
		Console();
		~Console();

		void ClearLog();

		void PushLogItem(stl::string_view str);
		void draw(float dt, const char* title, bool* p_open);
		void Flush();
		void Init();

		void ExecCommand(const char* command_line);

	private:
		static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
		int TextEditCallback(ImGuiInputTextCallbackData* data);
	};
};
