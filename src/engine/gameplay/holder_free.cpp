#include "pch.h"

using namespace Asura;

gameplay::holder_mode gameplay::holder_type = {};

static b2MouseJoint* current_contol_joint = nullptr;
static Physics::PhysicsBody* current_contol_body = nullptr;

static Physics::PhysicsBody* joint_contact_body = nullptr;
static Math::FVec2 joint_contact_point = {};

void gameplay::holder::free::Init()
{
}

void gameplay::holder::free::Tick()
{
    if (input::is_focused_on_ui()) 
    {
        return;
    }

    if (input::is_key_pressed(SDL_SCANCODE_MOUSE_X1))
    {
        Math::FVec2 mouse_position_absolute = Camera::screen_to_world(ImGui::GetMousePos());

        if (joint_contact_body == nullptr) 
        {
            joint_contact_body = Physics::HitTest(mouse_position_absolute);
            joint_contact_point = mouse_position_absolute;
        } 
        else 
        {
            const Physics::PhysicsBody* test_body = Physics::HitTest(mouse_position_absolute);
            if (test_body != nullptr && test_body != joint_contact_body && test_body->get_body_type() != Physics::body_type::ph_static)
            {
                constexpr float frequency_hz = 5.0f;
                constexpr float damping_ratio = 0.7f;

                b2DistanceJointDef jointDef;
                jointDef.Initialize(joint_contact_body->get_body(), test_body->get_body(), joint_contact_point, mouse_position_absolute);

                jointDef.collideConnected = true;
                b2LinearStiffness(jointDef.stiffness, jointDef.damping, frequency_hz, damping_ratio, jointDef.bodyA, jointDef.bodyB);

                Physics::GetWorld().CreateJoint(&jointDef);
                test_body->get_body()->SetAwake(true);
            }

            joint_contact_body = nullptr;
        }
    }

    static bool sound_started = false;

    if (input::is_key_pressed(SDL_SCANCODE_MOUSE_LEFT)) 
    {
        Math::FVec2 mouse_position_absolute = ImGui::GetMousePos();
        mouse_position_absolute = Camera::screen_to_world(mouse_position_absolute);

        if (current_contol_joint == nullptr) 
        {
            current_contol_body = Physics::HitTest(mouse_position_absolute);

            if (current_contol_body != nullptr && current_contol_body->get_body_type() != Physics::body_type::ph_static)
            {
                if (!sound_started)
                {
                    Audio::Start("click.ogg");

                    sound_started = true;
                }

                constexpr float frequency_hz = 60.0f;
                constexpr float damping_ratio = 1.f;
                b2MouseJointDef jd;

                jd.bodyA = Physics::GetGround()->get_body();
                jd.bodyB = current_contol_body->get_body();
                jd.target = mouse_position_absolute;
                jd.maxForce = 1000.0f * current_contol_body->get_body()->GetMass();
                b2LinearStiffness(jd.stiffness, jd.damping, frequency_hz, damping_ratio, jd.bodyA, jd.bodyB);

                current_contol_joint = dynamic_cast<b2MouseJoint*>(Physics::GetWorld().CreateJoint(&jd));
                current_contol_body->get_body()->SetAwake(true);
            }
            else 
            {
                current_contol_joint = nullptr;
            }
        }
        else 
        {
            if (!current_contol_body->IsDestroyed())
            {
                current_contol_joint->SetTarget(mouse_position_absolute);
            }
            else 
            {
                current_contol_joint = nullptr;
            }
        }
    }
    else
    {
        sound_started = false;
    }

    if (current_contol_joint != nullptr && !input::is_key_pressed(SDL_SCANCODE_MOUSE_LEFT)) 
    {
        Physics::GetWorld().DestroyJoint(current_contol_joint);
        current_contol_joint = nullptr;
        current_contol_body = nullptr;
    }
}

void gameplay::holder::free::Destroy()
{
}