#pragma once

namespace Asura::UI
{
    struct Console
    {
        char InputBuf[256] = {};
        ImVector<char*> Items;
        stl::hash_map<stl::string, stl::string> cmd_hint;

        ImVector<char*> History;
        int HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
        ImGuiTextFilter Filter;
        bool AutoScroll;
        bool ScrollToBottom;

        Console();

        ~Console();

        void clear_log();

        void push_log_item(stl::string_view str);
        void draw(float dt, const char* title, bool* p_open);
        void flush();
        void Init();

        void ExecCommand(const char* command_line);
        static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
        int TextEditCallback(ImGuiInputTextCallbackData* data);
    };
};
