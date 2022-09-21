#include "pch.h"

using namespace ark;
extern bool fullscreen_mode;
extern int window_width;
extern int window_height;

extern ui::UIConsole console;
bool show_console = false;
bool show_fps_counter = true;

// Test 

void
ui::init()
{
}

void
ui::tick(float dt)
{
    if (show_console) {
        console.draw(dt, "Arkane console", &show_console);
    }
    if (show_fps_counter) {

        ImGui::SetNextWindowPos({ float(window_width - 230), 5 });
        ImGui::SetNextWindowSize({300, 50});
        if (!ImGui::Begin("debug draw", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs))
        {
            ImGui::End();
            return;
        }

        ImGui::SameLine();
        ImGui::Text("FPS/DeltaTime: %.4f/%.4f", 1 / dt, dt);
        ImGui::End();
    }
}

int64_t 
ui::get_cmd_int(std::string_view str)
{
    if (str == "window_fullscreen") {
        return fullscreen_mode;
    }
    if (str == "window_width") {
        return window_width;
    }
    if (str == "window_height") {
        return window_height;
    }
    
    return -1;
}

void 
ui::push_console_string(std::string_view str)
{
    console.push_log_item(str);
}

void
ui::destroy()
{
   console.clear_log();
}
