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
extern std::unique_ptr<ui::console> console;

bool show_entity_inspector = false;
bool show_console = false;
bool show_fps_counter = true;

/*
template<typename Component>
void inspect_entity_component(stl::hash_map<stl::string, stl::string>& kv_storage, entt::entity ent)
{
    if (entities::contains<Component>(ent)) {
        entt::id_type id = entt::type_id<Component>().hash();
        auto& storage = (*entities::internal::get_registry().get().storage(id)).second;
        if constexpr (entities::is_flag_v<Component>) {
#ifdef __GNUC__
            int status = 0;
            char* string_ptr = abi::__cxa_demangle(typeid(Component).name(), 0, 0, &status);
            ImGui::Text("Flag: %s", string_ptr);
            free(string_ptr);
#else
            ImGui::Text("Flag: %s", typeid(Component).name());
#endif
        } else {
#ifdef __GNUC__
            int status = 0;
            char* string_ptr = abi::__cxa_demangle(typeid(Component).name(), 0, 0, &status);
            ImGui::Text("Component: %s", string_ptr);
            free(string_ptr);
#else
            ImGui::Text("Component: %s", typeid(Component).name());
#endif
            const Component* value_ptr = static_cast<const Component*>(storage.get(ent));
            if (value_ptr != nullptr) {
                value_ptr->string_serialize(kv_storage);
            }
        }
    }
}

template<typename... Args>
void inspect_entity(entt::entity ent)
{
    stl::hash_map<stl::string, stl::string> kv_storage;
    (inspect_entity_component<Args>(kv_storage, ent), ...);

    ImGui::Separator();
    for (const auto& [key, value] : kv_storage) {
        ImGui::Text("%s: %s", key.data(), value.data());
    }
}
*/

void
ui::init()
{
    std::filesystem::path font_dir = filesystem::get_content_dir();
    font_dir.append("fonts").append("RobotoMono-Regular.ttf");

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(font_dir.generic_string().c_str(), 18, nullptr, io.Fonts->GetGlyphRangesCyrillic());
}

void
ui::tick(float dt)
{
    OPTICK_EVENT("ui draw")

#ifndef ARKANE_SHIPPING 
    if (ImGui::IsKeyPressed(ImGuiKey_F2)) {
        show_entity_inspector = !show_entity_inspector;
    }
    
    if (ImGui::IsKeyPressed(ImGuiKey_F3)) {
        show_fps_counter = !show_fps_counter;
    }

    uint32_t fps_counter_size = 0;
    float navigation_bar_size = 30.f;
    static bool stat_enable = false;
    math::fvec2 cursor_pos = {ImGui::GetMousePos().x, ImGui::GetMousePos().y};
    if (show_console) {
		OPTICK_EVENT("ui console draw")
        ::console->draw(dt, "Arkane console", &show_console);
    } else { 
        if (show_fps_counter) {
            fps_counter_size = stat_enable ? 700 : 240;
            ImGui::SetNextWindowPos({ static_cast<float>(window_width - 400), navigation_bar_size });
            ImGui::SetNextWindowSize({400, static_cast<float>(fps_counter_size) });
            if (ImGui::Begin("debug draw", 0, ImGuiWindowFlags_NoDecoration)) {
                math::fvec2 wcursor_pos = camera::screen_to_world(cursor_pos);

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
                
                const auto& registry = entities::internal::get_registry().get();
                ImGui::Checkbox("Engine statistics", &stat_enable);
                ImGui::Checkbox("Debug draw", &physical_debug_draw);
                ImGui::Checkbox("Paused", &paused);
                ImGui::Checkbox("Use parallel", &use_parallel);
                ImGui::SliderFloat("Physics TPS", &target_physics_tps, 1.f, 120.f);
                ImGui::SliderFloat("Physics Hertz", &target_physics_hertz, 1.f, 120.f);
                ImGui::SliderFloat("Camera zoom", &cam_zoom, 1.f, 120.f);
                ImGui::SliderInt("Steps count", &target_steps_count, 1, 4);
                if (stat_enable) {
                    ImGui::Separator();
                    ImGui::Text("UI:");
                    ImGui::Text("  FocusId: %i", ImGui::GetFocusID());
                    ImGui::Text("  FocusScope: %i", ImGui::GetFocusScope());
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
                    ImGui::Text("  Bodies count: %i", physics::get_world().GetBodyCount());
                    ImGui::Separator();
                    ImGui::Text("Entities");
                    ImGui::Text("  Allocated: %d", static_cast<int>(registry.capacity()));
                    ImGui::Text("  Alive: %d", static_cast<int>(registry.alive()));
                    ImGui::Separator();
                    ImGui::Text("UI Info:");
                    ImGui::Text("  Camera position: %.1f, %.1f", camera::camera_position().x(), camera::camera_position().y());
                    ImGui::Text("  Cursor screen position: %.1f, %.1f", cursor_pos.x(), cursor_pos.y());
                    ImGui::Text("  Cursor world position: %.1f, %.1f", wcursor_pos.x(), wcursor_pos.y());
                }
            }
                
            ImGui::End();
        }

        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize({ static_cast<float>(window_width), navigation_bar_size });
        if (ImGui::Begin("NavigationBar", nullptr, ImGuiWindowFlags_NoDecoration)) {
            ImGui::BeginChild("ChildR", ImVec2(0, 260), true, ImGuiWindowFlags_MenuBar);
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Update directories")) {
                        resources::update_directories();
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Edit")) {
                    ImGui::MenuItem("Redo", "Ctrl + Shift + Z", nullptr, false);
                    ImGui::MenuItem("Undo", "Ctrl + Z", nullptr, false);
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("View")) {
                    ImGui::MenuItem("Entity Inspector", "F2", &show_entity_inspector);
                    ImGui::MenuItem("FPS Counter", "F3", &show_fps_counter);
                    ImGui::EndMenu();
                }
                
                if (ImGui::BeginMenu("Scene")) {
                    if (ImGui::MenuItem("Import scene", "Ctrl + O")) {
                        
                    }

                    if (ImGui::MenuItem("Export scene", "Ctrl + S")) {

                    }

                    if (ImGui::MenuItem("Close scene")) {
                        scene::close_scene();
                    }
                    
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Entities")) {
                    if (ImGui::MenuItem("Destroy all entities")) {
                        entities::free();
                    }

                    if (ImGui::MenuItem("Clear all entities")) {
                        entities::clear();
                    }

                    ImGui::Separator();

                    if (ImGui::MenuItem("Serialize game state")) {
                        entities::serialize_state("debug_game_state.bin");
                    }

                    if (ImGui::MenuItem("Deserialize game state")) {
                        entities::deserialize_state("debug_game_state.bin");
                    }

                    if (ImGui::MenuItem("String serialize game state")) {
                        entities::string_serialize_state("string_debug_game_state.ini");
                    }

                    if (ImGui::MenuItem("String deserialize game state")) {
                        entities::string_deserialize_state("string_debug_game_state.ini");
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            ImGui::EndChild();
            ImGui::End();
        }

         if (show_entity_inspector) {
             auto height_value = static_cast<float>(window_height - fps_counter_size);
             if (height_value < fps_counter_size) {
                 ImGui::SetNextWindowPos({ 0, navigation_bar_size });
                 ImGui::SetNextWindowSize({ 400, static_cast<float>(window_height - navigation_bar_size) });
             } else {
                 ImGui::SetNextWindowPos({ static_cast<float>(window_width - 400), static_cast<float>(navigation_bar_size + fps_counter_size) });
                 ImGui::SetNextWindowSize({ 400, static_cast<float>(window_height - fps_counter_size) });
             }

             if (ImGui::Begin("EntityInspector", nullptr, ImGuiWindowFlags_NoDecoration)) {
                 static entt::entity inspected_entity = entt::null;
                 if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                     auto phys_body = physics::hit_test(camera::screen_to_world(cursor_pos));
                     if (phys_body != nullptr) {
                         inspected_entity = entities::get_entity_from_body(phys_body->get_body()).get();
                     } else {
                         inspected_entity = entt::null;
                     }
                 }
         
                 ImGui::End();
             }
         }
    }

    auto current_ms_time = std::chrono::steady_clock::now().time_since_epoch().count() / 1000000;
    if (current_ms_time < (entities::get_last_serialize_time().count() / 1000000) + 2000) {
        ImGui::GetForegroundDrawList()->AddText(ImVec2(static_cast<float>(window_width) - 325, 8), ImColor(1.f, 1.f, 1.f), "Serialization/Deserialization complete");
    } else if (current_ms_time < (resources::get_last_update_time().count() / 1000000) + 2000) {
        ImGui::GetForegroundDrawList()->AddText(ImVec2(static_cast<float>(window_width) - 220, 8), ImColor(1.f, 1.f, 1.f), "Engine directories update");
    }
#endif
}

int64_t 
ui::get_cmd_int(stl::string_view str)
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
ui::push_console_string(stl::string_view str)
{
    ::console->push_log_item(str);
}

void
ui::destroy()
{
    ::console->flush();
    ::console->clear_log();
}
