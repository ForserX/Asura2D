#include "pch.h"

using namespace asura;

std::unique_ptr<ui::console> console;
input::on_key_change console_key_change;

using ui::console;

console::console()
{
    clear_log();
    memset(InputBuf, 0, sizeof(InputBuf));
    HistoryPos = -1;

    cmd_hint["help"] = "";
    cmd_hint["hide"] = "";
    cmd_hint["history"] = "";
    cmd_hint["clear"] = "";
    cmd_hint["physical_debug_draw"] = "1 , 0";
    cmd_hint["use_parallel"] = "1 , 0";
    cmd_hint["pause"] = {};
    cmd_hint["physics_tps"] = "0 - 120";
    cmd_hint["physics_hertz"] = "0 - 120";
    cmd_hint["window_maximized"] = "1, 0";
    cmd_hint["draw_fps"] = "1, 0";
    cmd_hint["window_style"] = "red, dark, white";
    cmd_hint["window_fullscreen"] = "1, 0";
    cmd_hint["window_width"] = "int";
    cmd_hint["window_height"] = "int";

    AutoScroll = true;
    ScrollToBottom = false;
}

console::~console()
{
    clear_log();

    for (int i = 0; i < History.Size; i++)
    {
        free(History[i]);
    }

    input::unsubscribe_key_event(console_key_change);
}

// Portable helpers
int  strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
void strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

void console::clear_log()
{
    for (int i = 0; i < Items.Size; i++)
        free(Items[i]);
    Items.clear();
}

void console::push_log_item(stl::string_view str)
{
    Items.push_back(strdup(str.data()));
}

void console::draw(float dt, const char* title, bool* p_open)
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
    ImGui::SameLine();
    ImGui::Text("FPS/DeltaTime: %.4f/%.4f", 1 / dt, dt);

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
        for (const auto&[command, hint] : cmd_hint)
        {
            if (strlen(InputBuf) == command.length()) 
            {
                skip = true;
                break;
            }

            if (command.find(InputBuf) != stl::string::npos)
            {
                stl::string print = command + stl::string(" (") + hint + ")";
                item_list[Iter] = strdup(print.c_str());
                Iter++;
            }
        }

        static int item_current = -1;
        if (!skip && Iter > 0)
        {
            const ImVec2 safe_pos = ImGui::GetCursorPos();
            ImVec2 pos = ImGui::GetCursorPos();
            pos.y -= Iter * (TextBoxPos.y - pos.y) + 5;
            ImGui::SetCursorPos(pos);

            ImGui::ListBox(" ", &item_current, item_list, Iter);
            ImGui::SetCursorPos(safe_pos);
        }

        if (item_current != -1) 
        {
            stl::string try_str = item_list[item_current];

            const size_t space_iter = try_str.find(' ');
            if (space_iter != stl::string::npos) {
                try_str = try_str.substr(0, space_iter);
            }

            try_str.copy(InputBuf, try_str.size());
            item_current = -1;
        }

        for (size_t del_iter = 0; del_iter < Iter; del_iter++) 
        {
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

#define CHECK_FROM_CMD(command, output) \
    if (strstr(command_line, command)) { \
        cmd = cmd.substr(stl::string(command).length()); \
        if (!cmd.empty()) { \
            output = (decltype(output))stl::stof(cmd); \
        } else { \
            debug::msg("Invalid parameter: '{}'\n", command_line); \
        } \
    }

#define CHECK_FROM_CMD_EX(command, output, callback) \
    if (strstr(command_line, command)) { \
        cmd = cmd.substr(stl::string(command).length()); \
        if (!cmd.empty()) { \
            output = (decltype(output))stl::stof(cmd); \
            callback(); \
        } else { \
            debug::msg("Invalid parameter: '{}'\n", command_line); \
        } \
    }

void console::ExecCommand(const char* command_line)
{
    debug::msg("# {} \n", command_line);

    // Insert into history. First find match and delete it so it can be pushed to the back.
    // This isn't trying to be smart or optimal.
    HistoryPos = -1;
    for (int i = History.Size - 1; i >= 0; i--)
        if (std::strcmp(History[i], command_line) == 0)
        {
            free(History[i]);
            History.erase(History.begin() + i);
            break;
        }
    History.push_back(strdup(command_line));

    stl::string cmd = command_line;
    std::erase_if(cmd, [](unsigned char x) {return std::isspace(x);});

    CHECK_FROM_CMD("physics_hertz",         target_physics_hertz);
    CHECK_FROM_CMD("physics_tps",           target_physics_tps);
    CHECK_FROM_CMD("draw_fps",              show_fps_counter);
    CHECK_FROM_CMD("physical_debug_draw",   physical_debug_draw);
    CHECK_FROM_CMD("use_parallel",          use_parallel);
    CHECK_FROM_CMD("pause",                 paused);

    CHECK_FROM_CMD_EX("window_height",      window_height,      window::change_resolution);
    CHECK_FROM_CMD_EX("window_width",       window_width,       window::change_resolution);
    CHECK_FROM_CMD_EX("window_maximized",   window_maximized,   window::change_window_mode);
    CHECK_FROM_CMD_EX("window_fullscreen",  fullscreen_mode,    window::change_fullscreen);
    
    // Process other command
    if (cmd == "clear")
    {
        clear_log();
    }
    else if (cmd == "hide")
    {
        show_console = false;
    }
    else if (cmd == "help")
    {
        debug::msg("Commands:");
        for (const auto& [key, tip] : cmd_hint)
        {
            debug::msg("- {} : {}", key, tip);
        }
    }
    else if (cmd == "history")
    {
        const int first = History.Size - 10;
        for (int i = first > 0 ? first : 0; i < History.Size; i++)
            debug::msg("{}: {}\n", i, History[i]);
    }
    else if (strstr(command_line, "window_style")) {
        cmd = cmd.substr(12);

        if (cmd == "red") {
            window_style = graphics::theme::style::red;
        }
        else if (cmd == "dark") {
            window_style = graphics::theme::style::dark;
        }
        else if (cmd == "white") {
            window_style = graphics::theme::style::white;
        }
        else {
            window_style = graphics::theme::style::invalid;
        }

        graphics::theme::change();
    }
    // On command input, we scroll to bottom even if AutoScroll==false
    ScrollToBottom = true;
}

int console::TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
    auto* console = static_cast<ui::console*>(data->UserData);
    return console->TextEditCallback(data);
}

int console::TextEditCallback(ImGuiInputTextCallbackData* data)
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
            {
                break;
            }
            word_start--;
        }

        // Build a list of candidates
        ImVector<const char*> candidates;
        for (const auto& [key, tip] : cmd_hint)
        {
            if (strnicmp(key.c_str(), word_start, (int)(word_end - word_start)) == 0)
                candidates.push_back(key.c_str());
        }

        if (candidates.Size == 0)
        {
            // No match
            debug::msg("No match for \"{}\"!\n", (int)(word_end - word_start), word_start);
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
            while (true)
            {
                int c = 0;
                bool all_candidates_matches = true;

                for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                {
                    if (i == 0)
                        c = toupper(candidates[i][match_len]);
                    else if (c == 0 || c != toupper(candidates[i][match_len]))
                        all_candidates_matches = false;
                }

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
            debug::msg("Possible matches: \n");
            for (int i = 0; i < candidates.Size; i++)
                debug::msg("- {}\n", candidates[i]);
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

void console::flush()
{
	std::filesystem::path cfg_path = filesystem::get_userdata_dir();
	cfg_path = cfg_path.append("user.cfg");
    filesystem::create_file(cfg_path);
    std::ofstream cfg(cfg_path);


    for (const auto& [cmd, hint] : cmd_hint)
    {
        if (hint.length() > 0)
        {
            cfg << cmd << " " << ui::get_cmd_int(cmd) << std::endl;
        }
    }
}

void console::init()
{
    console_key_change = input::subscribe_key_event(
        [](int16_t scan_code, input::key_state state)
        {
            if (scan_code == SDL_SCANCODE_GRAVE && state == input::key_state::press) 
            {
                show_console = !show_console;
            }
        }
    );

	std::filesystem::path cfg_path = filesystem::get_userdata_dir();
    cfg_path = cfg_path.append("user.cfg");
    if (!std::filesystem::exists(cfg_path))
    {
        return;
    }

    std::ifstream cfg(cfg_path);

    stl::string line;

    while (std::getline(cfg, line))
    {
        ExecCommand(line.c_str());
    }
}