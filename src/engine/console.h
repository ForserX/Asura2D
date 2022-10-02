﻿#pragma once

namespace ark::ui
{
    struct UIConsole
    {
        char InputBuf[256] = {};
        ImVector<char*> Items;
        ImVector<const char*> Commands;
        stl::hash_map<stl::string, stl::string> cmd_hint;

        ImVector<char*> History;
        int HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
        ImGuiTextFilter Filter;
        bool AutoScroll;
        bool ScrollToBottom;

        UIConsole();

        ~UIConsole();

        void clear_log();

        void push_log_item(stl::string_view str);
        void draw(float dt, const char* title, bool* p_open);
        void flush();
        void init();

        void ExecCommand(const char* command_line);
        static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
        int TextEditCallback(ImGuiInputTextCallbackData* data);
    };
};
