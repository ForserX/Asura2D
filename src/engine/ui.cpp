#include "pch.h"

using namespace ark;
extern bool fullscreen_mode;
extern int window_width;
extern int window_height;

extern ui::UIConsole console;
bool show_console = false;
bool show_fps_counter = true;

b2MouseJoint* TestMouseJoint = nullptr;
b2Body* ContactBody = nullptr;
ImVec2 ContactPoint = {};
// Test 

void
ui::init()
{
}

void
ui::tick(float dt)
{
    OPTICK_EVENT("ui draw")
    if (show_console) {
		OPTICK_EVENT("ui console draw")
        console.draw(dt, "Arkane console", &show_console);
    }
    
    if (show_fps_counter) {

        ImGui::SetNextWindowPos({ static_cast<float>(window_width - 230), 5 });
        ImGui::SetNextWindowSize({300, 200});
        if (!ImGui::Begin("debug draw", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs))
        {
            ImGui::End();
            return;
        }

        ImGui::SameLine();

        physics::get_physics_mutex().lock();
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            ImVec2 mousePositionAbsolute = ImGui::GetMousePos();
            mousePositionAbsolute.y = static_cast<float>(get_cmd_int("window_height")) - mousePositionAbsolute.y;

            if (ContactBody == nullptr) {
                ContactBody = physics::hit_test(mousePositionAbsolute);
                ContactPoint = mousePositionAbsolute;
            }
            else {
                b2Body* TestBody = physics::hit_test(mousePositionAbsolute);

                if (TestBody != nullptr && TestBody != ContactBody)
                {
                    constexpr float frequency_hz = 5.0f;
                    constexpr float damping_ratio = 0.7f;

                    b2DistanceJointDef jointDef;
                    jointDef.Initialize(ContactBody, TestBody, *(b2Vec2*)&ContactPoint,
                        *(b2Vec2*)&mousePositionAbsolute);

                    jointDef.collideConnected = true;
                    b2LinearStiffness(jointDef.stiffness, jointDef.damping, frequency_hz, damping_ratio, jointDef.bodyA, jointDef.bodyB);

                    physics::get_world().CreateJoint(&jointDef);
                    TestBody->SetAwake(true);

                    ContactBody = nullptr;
                }
            }
        }

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
        
        physics::get_physics_mutex().unlock();

        if (!ImGui::IsWindowFocused()) {
            ImGui::SetWindowFocus();
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
