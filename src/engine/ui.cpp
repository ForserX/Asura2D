#include "pch.h"

using namespace ark;
extern bool fullscreen_mode;
extern int window_width;
extern int window_height;
extern bool window_maximized;
extern float target_physics_tps;
extern float target_physics_hertz;

extern ui::UIConsole console;
bool show_console = false;
bool show_fps_counter = true;

void
ui::init()
{
    console.init();
}

void
ui::tick(float dt)
{
    OPTICK_EVENT("ui draw")
    if (show_console) {
		OPTICK_EVENT("ui console draw")
        console.draw(dt, "Arkane console", &show_console);
    }
    
    if (!show_console && show_fps_counter) {

        ImGui::SetNextWindowPos({ static_cast<float>(window_width - 300), 5 });
        ImGui::SetNextWindowSize({300, 200});
        if (!ImGui::Begin("debug draw", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration))
        {
            ImGui::End();
            return;
        }

        ImGui::SameLine();

        if (!ImGui::IsWindowFocused()) {
            ImGui::SetWindowFocus();
        }

        ark_float_vec2 cursor_pos = ImGui::GetMousePos();
        const auto& registry = entities::get_registry().get();
        ImGui::Text("Render FPS/DeltaTime: %.4f/%.4f", 1.f / dt, dt);
        ImGui::Text("Physics TPS/DeltaTime: %.4f/%.4f", 1.f / physics_delta, physics_delta);
        ImGui::Text("Physics:");
        ImGui::Text("   Bodies count: %i", physics::get_world().GetBodyCount());
        ImGui::Text("Entities");
        ImGui::Text("   Allocated: %d", registry.capacity());
        ImGui::Text("   Alive: %d", registry.alive());
        ImGui::Text("UI Info:");
        ImGui::Text("   Camera position: %.1f, %.1f", camera::camera_postion().x, camera::camera_postion().y);
        ImGui::Text("   Cursor position: %.1f, %.1f", cursor_pos.x, cursor_pos.y);
        ImGui::SliderFloat("Physics TPS", &target_physics_tps, 1.f, 120.f);
        ImGui::SliderFloat("Physics Hertz", &target_physics_hertz, 1.f, 120.f);
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

    if (str == "draw_fps") {
        return show_fps_counter;
    }
    
    if (str == "physics_tps") {
        return target_physics_tps;
    }

    if (str == "physics_hertz") {
        return target_physics_hertz;
    }

    if (str == "window_maximized") {
        return window_maximized;
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
   console.flush();
   console.clear_log();
}
