#include "pch.h"

using namespace ark;
extern bool fullscreen_mode;
extern int window_width;
extern int window_height;

extern ui::UIConsole console;
bool show_console = false;
bool show_fps_counter = true;

b2MouseJoint* TestMouseJoint = nullptr;
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

        ImGui::SetNextWindowPos({ static_cast<float>(window_width - 230), 5 });
        ImGui::SetNextWindowSize({300, 200});
        if (!ImGui::Begin("debug draw", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs))
        {
            ImGui::End();
            return;
        }

        ImGui::SameLine();

        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            ImVec2 mousePositionAbsolute = ImGui::GetMousePos();
            mousePositionAbsolute.y = static_cast<float>(get_cmd_int("window_height")) - mousePositionAbsolute.y;

            if (TestMouseJoint == nullptr) {
                b2Body* TestBody = physics::hit_test(mousePositionAbsolute);

                if (TestBody != nullptr) {
                    constexpr float frequency_hz = 5.0f;
                    constexpr float damping_ratio = 0.7f;

                    b2MouseJointDef jd;
                    jd.bodyA = &physics::get_world().GetBodyList()[0];
                    jd.bodyB = TestBody;
                    jd.target = *reinterpret_cast<b2Vec2*>(&mousePositionAbsolute);
                    jd.maxForce = 1000.0f * TestBody->GetMass();
                    b2LinearStiffness(jd.stiffness, jd.damping, frequency_hz, damping_ratio, jd.bodyA, jd.bodyB);

                    TestMouseJoint = dynamic_cast<b2MouseJoint*>(physics::get_world().CreateJoint(&jd));
                    TestBody->SetAwake(true);
                }
            }
            else
            {
                TestMouseJoint->SetTarget(*reinterpret_cast<b2Vec2*>(&mousePositionAbsolute));
            }
        }

        if (TestMouseJoint != nullptr && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            physics::get_world().DestroyJoint(TestMouseJoint);
            TestMouseJoint = nullptr;
        }

        const auto& registry = entities::get_registry().get();
        ImGui::Text("FPS/DeltaTime: %.4f/%.4f", 1.f / dt, dt);
        ImGui::Text("Physics:");
        ImGui::Text("   Bodies count: %i", physics::get_world().GetBodyCount());
        ImGui::Text("Entities");
        ImGui::Text("   Allocated: %d", registry.capacity());
        ImGui::Text("   Alive: %d", registry.alive());
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
