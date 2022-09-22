#include "pch.h"

using namespace ark::systems;


b2MouseJoint* TestMouseJoint = nullptr;
b2Body* ContactBody = nullptr;
ImVec2 ContactPoint = {};
// Test 

void
physics_mouse_joint_system::init()
{
}

void
physics_mouse_joint_system::reset()
{
}

void
physics_mouse_joint_system::tick(registry& reg, float dt)
{
	 if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            ImVec2 mousePositionAbsolute = ImGui::GetMousePos();
            mousePositionAbsolute.y = static_cast<float>(ui::get_cmd_int("window_height")) - mousePositionAbsolute.y;

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
            mousePositionAbsolute.y = static_cast<float>(ui::get_cmd_int("window_height")) - mousePositionAbsolute.y;

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
}
