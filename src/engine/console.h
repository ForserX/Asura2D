#pragma once

namespace ark::ui
{
    struct UIConsole
    {
        char InputBuf[256] = {};
        ImVector<char*> Items;
        ImVector<const char*> Commands;
        std::unordered_map<std::string, std::string> cmd_hint;

        ImVector<char*> History;
        int HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
        ImGuiTextFilter Filter;
        bool AutoScroll;
        bool ScrollToBottom;

        UIConsole();

        ~UIConsole();

        void clear_log();

        void push_log_item(std::string_view str);
        void draw(const char* title, bool* p_open);


        void ExecCommand(const char* command_line);
        static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
        int TextEditCallback(ImGuiInputTextCallbackData* data);
    };
};
