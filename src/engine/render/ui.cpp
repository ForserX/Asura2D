#include "pch.h"
#include "../editor/editor_common.h"

using namespace Asura;
using namespace Asura::GamePlay;

bool show_entity_inspector = false;
bool show_console = false;
bool show_fps_counter = true;

static float OldVolume = Volume;

void UI::Init()
{
    FileSystem::Path font_dir = FileSystem::ContentDir();
    font_dir.append("fonts").append("RobotoMono-Regular.ttf");

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(font_dir.generic_string().c_str(), 18, nullptr, io.Fonts->GetGlyphRangesCyrillic());
}

void UI::Tick(float dt)
{
    OPTICK_EVENT("UI draw");

    if (is_editor_mode)
    {
        Editor::UI::Destroy();
        return;
    }

#ifndef ARKANE_SHIPPING 
    if (ImGui::IsKeyPressed(ImGuiKey_F2)) 
    {
        show_entity_inspector = !show_entity_inspector;
    }
    
    if (ImGui::IsKeyPressed(ImGuiKey_F3)) 
    {
        show_fps_counter = !show_fps_counter;
    }

    uint32_t fps_counter_size = 0;
    float navigation_bar_size = 30.f;
    static bool stat_enable = false;
    Math::FVec2 cursor_pos = {ImGui::GetMousePos().x, ImGui::GetMousePos().y};

    if (show_console) 
    {
		OPTICK_EVENT("UI Console draw")
        ::console->draw(dt, "Arkane Console", &show_console);
    } 
    else 
    { 
        if (show_fps_counter) 
        {
            fps_counter_size = stat_enable ? 700 : 240;
            ImGui::SetNextWindowPos({ static_cast<float>(window_width - 400), navigation_bar_size });
            ImGui::SetNextWindowSize({400, static_cast<float>(fps_counter_size) });

            if (ImGui::Begin("Debug draw", 0, ImGuiWindowFlags_NoDecoration))
            {
                Math::FVec2 wcursor_pos = Camera::Screen2World(cursor_pos);

                const float draw_fps = 1.f / dt;
                const float draw_ms = dt * 1000.f;
                
                const float phys_tps = 1.f / physics_delta;
                const float phys_ms = physics_delta * 1000.f;
                const float phys_real_tps =  1.f / physics_real_delta;
                const float phys_real_dt = physics_real_delta * 1000.f;
                const float phys_load_percent = (physics_real_delta / (1.f / target_physics_tps));
                
                const float scheduler_tps = 1.f / scheduler_delta;
                const float scheduler_ms = scheduler_delta * 1000.f;
                const float real_scheduler_tps = 1.f / scheduler_real_delta;
                const float real_scheduler_ms = scheduler_real_delta * 1000.f;
                
                const auto& registry = Entities::internal::GetRegistry().Get();
                ImGui::Checkbox("Engine statistics", &stat_enable);
                ImGui::Checkbox("Debug draw", &physical_debug_draw);
                ImGui::Checkbox("Paused", &paused);
                ImGui::Checkbox("Use parallel", &use_parallel);
                ImGui::SliderFloat("Physics TPS", &target_physics_tps, 1.f, 120.f);
                ImGui::SliderFloat("Physics Hertz", &target_physics_hertz, 1.f, 120.f);
                ImGui::SliderFloat("Camera zoom", &cam_zoom, 1.f, 120.f);
                ImGui::SliderFloat("Volume", &Volume, 0.f, 1.f);
                ImGui::SliderInt("Steps count", &target_steps_count, 1, 4);

                if (stat_enable) 
                {
                    ImGui::Separator();
                    ImGui::Text("UI:");
                    ImGui::Text("  FocusId: %i", ImGui::GetFocusID());
                    ImGui::Text("  FocusScope: %i", ImGui::GetCurrentFocusScope());
                    ImGui::Separator();
                    ImGui::Text("Game:");
                    ImGui::Text("  TPS/dt: %.4f/%3.3fms", draw_fps, draw_ms);
                    ImGui::Separator();
                    ImGui::Text("Scheduler:");
                    ImGui::Text("  TPS/dt: %.4f/%3.3fms", scheduler_tps, scheduler_ms);
                    ImGui::Text("  Real TPS/dt: %.4f/%3.3fms", real_scheduler_tps, real_scheduler_ms);
                    ImGui::Separator();
                    ImGui::Text("Physics:");
                    ImGui::Text("  TPS/dt: %.4f/%3.3fms", phys_tps, phys_ms);
                    ImGui::Text("  Real TPS/dt: %.4f/%3.3fms", phys_real_tps, phys_real_dt);
                    ImGui::Text("  Physics thread load: %3.0f%%", phys_load_percent * 100.f);
                    ImGui::ProgressBar(phys_load_percent);
                    ImGui::Text("  Bodies count: %i", Physics::GetWorld().GetBodyCount());
                    ImGui::Separator();
                    ImGui::Text("Entities");
                    ImGui::Text("  Allocated: %d", static_cast<int>(registry.capacity()));
                    ImGui::Text("  Alive: %d", static_cast<int>(registry.alive()));
                    ImGui::Separator();
                    ImGui::Text("UI Info:");
                    ImGui::Text("  Camera position: %.1f, %.1f", Camera::Position().x, Camera::Position().y);
                    ImGui::Text("  Cursor screen position: %.1f, %.1f", cursor_pos.x, cursor_pos.y);
                    ImGui::Text("  Cursor world position: %.1f, %.1f", wcursor_pos.x, wcursor_pos.y);
                }
            }
                
            ImGui::End();
        }

        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize({ static_cast<float>(window_width), navigation_bar_size });
        if (ImGui::Begin("NavigationBar", nullptr, ImGuiWindowFlags_NoDecoration))
        {
            ImGui::BeginChild("ChildR", ImVec2(0, 260), true, ImGuiWindowFlags_MenuBar);
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File")) 
                {
                    if (ImGui::MenuItem("Update directories"))
                    {
                        ResourcesManager::UpdateDirs();
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Edit"))
                {
                    ImGui::MenuItem("Redo", "Ctrl + Shift + Z", nullptr, false);
                    ImGui::MenuItem("Undo", "Ctrl + Z", nullptr, false);
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("View")) 
                {
                    ImGui::MenuItem("Entity Inspector", "F2", &show_entity_inspector);
                    ImGui::MenuItem("FPS Counter", "F3", &show_fps_counter);
                    ImGui::EndMenu();
                }
                
                if (ImGui::BeginMenu("Scene")) 
                {
                    if (ImGui::MenuItem("Import Scene", "Ctrl + O")) 
                    {
                        
                    }

                    if (ImGui::MenuItem("Export Scene", "Ctrl + S")) 
                    {

                    }

                    if (ImGui::MenuItem("Close Scene")) 
                    {
                        Scene::Close();
                    }
                    
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Entities")) 
                {
                    if (ImGui::MenuItem("Destroy all Entities"))
                    {
                        Entities::Free();
                    }

                    if (ImGui::MenuItem("Clear all Entities")) 
                    {
                        Entities::Clear();
                    }

                    ImGui::Separator();

                    if (ImGui::MenuItem("Serialize game state")) 
                    {
                        Entities::serialize_state("debug_game_state.bin");
                    }

                    if (ImGui::MenuItem("Deserialize game state")) 
                    {
                        Entities::deserialize_state("debug_game_state.bin");
                    }

                    if (ImGui::MenuItem("String serialize game state")) 
                    {
                        Entities::string_serialize_state("string_debug_game_state.ini");
                    }

                    if (ImGui::MenuItem("String deserialize game state")) 
                    {
                        Entities::string_deserialize_state("string_debug_game_state.ini");
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            ImGui::EndChild();
            ImGui::End();
        }

         if (show_entity_inspector) 
         {
             auto height_value = static_cast<float>(window_height - fps_counter_size);
             if (height_value < fps_counter_size) 
             {
                 ImGui::SetNextWindowPos({ 0, navigation_bar_size });
                 ImGui::SetNextWindowSize({ 400, static_cast<float>(window_height - navigation_bar_size) });
             } 
             else 
             {
                 ImGui::SetNextWindowPos({ static_cast<float>(window_width - 400), static_cast<float>(navigation_bar_size + fps_counter_size) });
                 ImGui::SetNextWindowSize({ 400, static_cast<float>(window_height - fps_counter_size) });
             }

             if (ImGui::Begin("EntityInspector", nullptr, ImGuiWindowFlags_NoDecoration))
             {
                 static entt::entity inspected_entity = entt::null;
                 if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                 {
                     auto phys_body = Physics::HitTest(Camera::Screen2World(cursor_pos));

                     if (phys_body != nullptr) 
                     {
                         inspected_entity = Entities::GetEntityByBbody(phys_body->get_body());
                     } 
                     else 
                     {
                         inspected_entity = entt::null;
                     }
                 }
         
                 ImGui::End();
             }
         }
    }

    auto current_ms_time = std::chrono::steady_clock::now().time_since_epoch().count() / 1000000;
    if (current_ms_time < (Entities::get_last_serialize_time().count() / 1000000) + 2000) 
    {
        ImGui::GetForegroundDrawList()->AddText(ImVec2(static_cast<float>(window_width) - 325, 8), ImColor(1.f, 1.f, 1.f), "Serialization/Deserialization complete");
    } 
    else if (current_ms_time < (ResourcesManager::LastUpdateTime().count() / 1000000) + 2000)
    {
        ImGui::GetForegroundDrawList()->AddText(ImVec2(static_cast<float>(window_width) - 220, 8), ImColor(1.f, 1.f, 1.f), "Engine directories update");
    }
#endif

    if (OldVolume != Volume)
    {
        Audio::UpdateVolume();
    }
}

int64_t UI::GetCmdInt(stl::string_view str)
{
    if (str == "window_fullscreen") 
    {
        return fullscreen_mode;
    }

    if (str == "window_width")
    {
        return window_width;
    }

    if (str == "window_height") 
    {
        return window_height;
    }

    if (str == "draw_fps") 
    {
        return show_fps_counter;
    }
    
    if (str == "physics_tps")
    {
        return target_physics_tps;
    }

    if (str == "physics_hertz")
    {
        return target_physics_hertz;
    }

    if (str == "window_maximized")
    {
        return window_maximized;
    }

    if (str == "physical_debug_draw")
    {
        return physical_debug_draw;
    }
    
    return -1;
}

float Asura::UI::GetCmdFlt(stl::string_view key)
{
    if (key == "volume")
    {
        return Volume;
    }

    return -1;
}

stl::string Asura::UI::GetCmdStr(stl::string_view key)
{
    if (key == "window_style")
    {
        switch (window_style)
        {
        case Graphics::theme::style::red:   return "red";
        case Graphics::theme::style::dark:  return "dark";
        case Graphics::theme::style::white: return "white";
        }
    }

    return stl::string("-1");
}

void UI::push_console_string(stl::string_view str)
{
    ::console->PushLogItem(str);
}

void UI::Destroy()
{
    ::console->Flush();
    ::console->ClearLog();
}