#include "pch.h"
#include "console.h"

using namespace ark::ui;
extern bool fullscreen_mode;
extern int window_width;
extern int window_height;

UIConsole::UIConsole()
{
    clear_log();
    memset(InputBuf, 0, sizeof(InputBuf));
    HistoryPos = -1;

    // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
    Commands.push_back("help");
    Commands.push_back("history");
    Commands.push_back("clear");
    Commands.push_back("window_fullscreen");
    Commands.push_back("window_width");
    Commands.push_back("window_height");
    AutoScroll = true;
    ScrollToBottom = false;
}

UIConsole::~UIConsole()
{
    clear_log();
    for (int i = 0; i < History.Size; i++) {
        free(History[i]);
    }
}

// Portable helpers
int  strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
void strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

void UIConsole::clear_log()
{
    for (int i = 0; i < Items.Size; i++)
        free(Items[i]);
    Items.clear();
}

void UIConsole::push_log_item(std::string_view str)
{
    Items.push_back(strdup(str.data()));
}

void UIConsole::draw(const char* title, bool* p_open)
{
    auto& io = ImGui::GetIO();

    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize(io.DisplaySize);
    if (!ImGui::Begin(title, p_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::End();
        return;
    }

    // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
    // So e.g. IsItemHovered() will return true when hovering the title bar.
    // Here we create a context menu only available from the title bar.
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Close Console"))
            *p_open = false;
        ImGui::EndPopup();
    }

    const bool copy_to_clipboard = ImGui::SmallButton("Copy");
    //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

    ImGui::Separator();

    // Options menu
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &AutoScroll);
        ImGui::EndPopup();
    }

    // Options, Filter
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");

    ImGui::Separator();

    // Reserve enough left-over height for 1 separator + 1 input text
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::Selectable("Clear")) clear_log();
        ImGui::EndPopup();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
    if (copy_to_clipboard)
        ImGui::LogToClipboard();
    for (int i = 0; i < Items.Size; i++)
    {
        const char* item = Items[i];
        if (!Filter.PassFilter(item))
            continue;

        // Normally you would store more information in your item than just a string.
        // (e.g. make Items[] an array of structure, store color/type etc.)
        ImVec4 color;
        bool has_color = false;
        if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
        else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
        if (has_color)
            ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(item);
        if (has_color)
            ImGui::PopStyleColor();
    }
    if (copy_to_clipboard)
        ImGui::LogFinish();

    if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
        ImGui::SetScrollHereY(1.0f);
    ScrollToBottom = false;

    ImGui::PopStyleVar();
    ImGui::EndChild();

    static ImVec2 TextBoxPos;
    ImGui::Separator();

    if (InputBuf[0] && strlen(InputBuf) > 0)
    {
        bool skip = false;
        const char** item_list = new const char* [15];
        size_t Iter = 0;
        for (const char* command : Commands)
        {
            if (strlen(InputBuf) == strlen(command)) {
                skip = true;
                break;
            }
            if (strstr(command, InputBuf))
            {
                item_list[Iter] = strdup(command);
                Iter++;
            }
        }

        static int item_current = -1;
        if (!skip && Iter > 0)
        {
            ImVec2 safe_pos = ImGui::GetCursorPos();
            ImVec2 pos = ImGui::GetCursorPos();
            pos.y -= Iter * (TextBoxPos.y - pos.y) + 5;
            ImGui::SetCursorPos(pos);

            ImGui::ListBox(" ", &item_current, item_list, Iter);
            ImGui::SetCursorPos(safe_pos);
        }

        if (item_current != -1) {
            strcpy(InputBuf, item_list[item_current]);
            item_current = -1;
        }

        for (size_t del_iter = 0; del_iter < Iter; del_iter++) {
            delete(item_list[del_iter]);
        }

        delete[](item_list);
    }

    // Command-line
    bool reclaim_focus = false;
    constexpr ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
    if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
    {
        char* s = InputBuf;
        strtrim(s);
        if (s[0])
            ExecCommand(s);
        strcpy(s, "");
        reclaim_focus = true;
    }

    TextBoxPos = ImGui::GetCursorPos();

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();
}

void UIConsole::ExecCommand(const char* command_line)
{
    debug::msg("# {} \n", command_line);

    // Insert into history. First find match and delete it so it can be pushed to the back.
    // This isn't trying to be smart or optimal.
    HistoryPos = -1;
    for (int i = History.Size - 1; i >= 0; i--)
        if (stricmp(History[i], command_line) == 0)
        {
            free(History[i]);
            History.erase(History.begin() + i);
            break;
        }
    History.push_back(strdup(command_line));

    std::string cmd = command_line;
    // Process command
    if (cmd == "clear")
    {
        clear_log();
    }
    else if (cmd == "help")
    {
        debug::msg("Commands:");
        for (int i = 0; i < Commands.Size; i++)
            debug::msg("- {}", Commands[i]);
    }
    else if (cmd == "history")
    {
        const int first = History.Size - 10;
        for (int i = first > 0 ? first : 0; i < History.Size; i++)
            debug::msg("%3d: %s\n", i, History[i]);
    }
    else if (strstr(command_line, "window_height")) {
        std::erase_if(cmd, [](unsigned char x) {return std::isspace(x);});
        cmd = cmd.substr(13);
        window_height = std::stoi(cmd);
        window::change_resolution();
    }
    else if (strstr(command_line, "window_width")) {
        std::erase_if(cmd, [](unsigned char x) {return std::isspace(x);});
        cmd = cmd.substr(12);
        window_width = std::stoi(cmd);
        window::change_resolution();
    }
    else if (strstr(command_line, "window_fullscreen")) {
        std::erase_if(cmd, [](unsigned char x) {return std::isspace(x);});
        cmd = cmd.substr(17);
        fullscreen_mode = !!std::stoi(cmd);
        window::change_fullscreen();
    }
    else
    {
        debug::msg("Unknown command: '%s'\n", command_line);
    }

    // On command input, we scroll to bottom even if AutoScroll==false
    ScrollToBottom = true;
}

int UIConsole::TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
    auto* console = static_cast<UIConsole*>(data->UserData);
    return console->TextEditCallback(data);
}

int UIConsole::TextEditCallback(ImGuiInputTextCallbackData* data)
{
    switch (data->EventFlag)
    {
    case ImGuiInputTextFlags_CallbackCompletion:
    {
        // Example of TEXT COMPLETION

        // Locate beginning of current word
        const char* word_end = data->Buf + data->CursorPos;
        const char* word_start = word_end;
        while (word_start > data->Buf)
        {
            const char c = word_start[-1];
            if (c == ' ' || c == '\t' || c == ',' || c == ';')
                break;
            word_start--;
        }

        // Build a list of candidates
        ImVector<const char*> candidates;
        for (int i = 0; i < Commands.Size; i++)
            if (strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
                candidates.push_back(Commands[i]);

        if (candidates.Size == 0)
        {
            // No match
            debug::msg("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
        }
        else if (candidates.Size == 1)
        {
            // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
            data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
            data->InsertChars(data->CursorPos, candidates[0]);
            data->InsertChars(data->CursorPos, " ");
        }
        else
        {
            // Multiple matches. Complete as much as we can..
            // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
            int match_len = static_cast<int>(word_end - word_start);
            for (;;)
            {
                int c = 0;
                bool all_candidates_matches = true;
                for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                    if (i == 0)
                        c = toupper(candidates[i][match_len]);
                    else if (c == 0 || c != toupper(candidates[i][match_len]))
                        all_candidates_matches = false;
                if (!all_candidates_matches)
                    break;
                match_len++;
            }

            if (match_len > 0)
            {
                data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
            }

            // List matches
            debug::msg("Possible matches:\n");
            for (int i = 0; i < candidates.Size; i++)
                debug::msg("- %s\n", candidates[i]);
        }

        break;
    }
    case ImGuiInputTextFlags_CallbackHistory:
    {
        // Example of HISTORY
        const int prev_history_pos = HistoryPos;
        if (data->EventKey == ImGuiKey_UpArrow)
        {
            if (HistoryPos == -1)
                HistoryPos = History.Size - 1;
            else if (HistoryPos > 0)
                HistoryPos--;
        }
        else if (data->EventKey == ImGuiKey_DownArrow)
        {
            if (HistoryPos != -1)
                if (++HistoryPos >= History.Size)
                    HistoryPos = -1;
        }

        // A better implementation would preserve the data on the current input line along with cursor position.
        if (prev_history_pos != HistoryPos)
        {
            const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, history_str);
        }
    }
    default:
        break;
    }

    return 0;
}

UIConsole console;