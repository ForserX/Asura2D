#include "pch.h"

using namespace ark;
extern bool fullscreen_mode;
extern int window_width;
extern int window_height;
extern bool window_maximized;
extern bool physical_debug_draw;
extern float target_physics_tps;
extern float target_physics_hertz;
extern float cam_zoom;
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
        ImGui::SetNextWindowSize({300, 400});
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
        ark_float_vec2 wcursor_pos = camera::screen_to_world(cursor_pos);
        const auto& registry = entities::get_registry().get();
        ImGui::Text("Controls:");
        ImGui::Checkbox("Debug draw", &physical_debug_draw);
        ImGui::SliderFloat("Physics TPS", &target_physics_tps, 1.f, 120.f);
        ImGui::SliderFloat("Physics Hertz", &target_physics_hertz, 1.f, 120.f);
        ImGui::SliderFloat("Camera zoom", &cam_zoom, 1.f, 120.f);
        ImGui::Text("Render:");
        ImGui::Text("   FPS/DeltaTime: %.4f/%.4f", 1.f / dt, dt);
        ImGui::Text("Physics:");
        ImGui::Text("   TPS/DeltaTime: %.4f/%.4f", 1.f / physics_delta, physics_delta);
        ImGui::Text("   Bodies count: %i", physics::get_world().GetBodyCount());
        ImGui::Text("Entities");
        ImGui::Text("   Allocated: %d", registry.capacity());
        ImGui::Text("   Alive: %d", registry.alive());
        ImGui::Text("UI Info:");
        ImGui::Text("   Camera position: %.1f, %.1f", camera::camera_position().x, camera::camera_position().y);
        ImGui::Text("   Cursor screen position: %.1f, %.1f", cursor_pos.x, cursor_pos.y);
        ImGui::Text("   Cursor world position: %.1f, %.1f", wcursor_pos.x, wcursor_pos.y);
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

    if (str == "physical_debug_draw") {
        return physical_debug_draw;
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
