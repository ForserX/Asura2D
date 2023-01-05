#include "pch.h"

using namespace Asura;

static b2MouseJoint* ControlJoint = nullptr;
static Physics::PhysicsBody* ContolBody = nullptr;
static Physics::PhysicsBody* EnttContolBody = nullptr;

void GamePlay::Holder::PlayerFree::Init()
{
}

void GamePlay::Holder::PlayerFree::Tick()
{
    if (Input::IsKeyPressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        Math::FVec2 mouse_position_absolute = ImGui::GetMousePos();
        mouse_position_absolute = GamePlay::Camera::Screen2World(mouse_position_absolute);

        if (ControlJoint == nullptr)
        {
            ContolBody = Physics::HitTest(mouse_position_absolute);

            if (ContolBody != nullptr && *EnttContolBody != *ContolBody)
            {
                ContolBody = nullptr;
                return;
            }

            if (ContolBody != nullptr && ContolBody->GetType() != Physics::BodyType::Static)
            {
                constexpr float frequency_hz = 60.0f;
                constexpr float damping_ratio = 1.f;
                b2MouseJointDef jd;

                jd.bodyA = Physics::GetGround()->get_body();
                jd.bodyB = ContolBody->get_body();
                jd.target = mouse_position_absolute;
                jd.maxForce = 1000.0f * ContolBody->get_body()->GetMass();
                b2LinearStiffness(jd.stiffness, jd.damping, frequency_hz, damping_ratio, jd.bodyA, jd.bodyB);

                ControlJoint = dynamic_cast<b2MouseJoint*>(Physics::GetWorld().GetWorld().CreateJoint(&jd));
                ContolBody->get_body()->SetAwake(true);
            }
            else
            {
                ControlJoint = nullptr;
            }
        }
        else
        {
            if (!ContolBody->IsDestroyed())
            {
                ControlJoint->SetTarget(mouse_position_absolute);
            }
            else
            {
                ControlJoint = nullptr;
            }
        }
    }

    if (ControlJoint != nullptr && !Input::IsKeyPressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        Physics::GetWorld().GetWorld().DestroyJoint(ControlJoint);
        ControlJoint = nullptr;
        ContolBody = nullptr;
    }

}

void GamePlay::Holder::PlayerFree::Destroy()
{
}

void GamePlay::Holder::PlayerFree::Bind(EntityBase Entity)
{
    auto TryEntt = Entities::TryGet<Entities::physics_body_component>(Entity);

    game_assert(TryEntt != nullptr, "Incorrect entity!", return);
    EnttContolBody = TryEntt->body;
}