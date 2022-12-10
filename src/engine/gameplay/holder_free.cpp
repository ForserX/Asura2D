#include "pch.h"

using namespace asura;

gameplay::holder_mode gameplay::holder_type = {};

static b2MouseJoint* current_contol_joint = nullptr;
static physics::physics_body* current_contol_body = nullptr;

static physics::physics_body* joint_contact_body = nullptr;
static math::fvec2 joint_contact_point = {};

void gameplay::holder::free::init()
{
}

void gameplay::holder::free::tick()
{
    if (input::is_focused_on_ui()) 
    {
        return;
    }

    if (input::is_key_pressed(SDL_SCANCODE_MOUSE_X1))
    {
        math::fvec2 mouse_position_absolute = camera::screen_to_world(ImGui::GetMousePos());

        if (joint_contact_body == nullptr) 
        {
            joint_contact_body = physics::hit_test(mouse_position_absolute);
            joint_contact_point = mouse_position_absolute;
        } 
        else 
        {
            const physics::physics_body* test_body = physics::hit_test(mouse_position_absolute);
            if (test_body != nullptr && test_body != joint_contact_body && test_body->get_body_type() != physics::body_type::ph_static)
            {
                constexpr float frequency_hz = 5.0f;
                constexpr float damping_ratio = 0.7f;

                b2DistanceJointDef jointDef;
                jointDef.Initialize(joint_contact_body->get_body(), test_body->get_body(), joint_contact_point, mouse_position_absolute);

                jointDef.collideConnected = true;
                b2LinearStiffness(jointDef.stiffness, jointDef.damping, frequency_hz, damping_ratio, jointDef.bodyA, jointDef.bodyB);

                physics::get_world().CreateJoint(&jointDef);
                test_body->get_body()->SetAwake(true);
            }

            joint_contact_body = nullptr;
        }
    }

    static bool sound_started = false;

    if (input::is_key_pressed(SDL_SCANCODE_MOUSE_LEFT)) 
    {
        math::fvec2 mouse_position_absolute = ImGui::GetMousePos();
        mouse_position_absolute = camera::screen_to_world(mouse_position_absolute);

        if (current_contol_joint == nullptr) 
        {
            current_contol_body = physics::hit_test(mouse_position_absolute);

            if (current_contol_body != nullptr && current_contol_body->get_body_type() != physics::body_type::ph_static)
            {
                if (!sound_started)
                {
                    std::filesystem::path snd_path = filesystem::get_content_dir();
                    snd_path.append("sound").append("click.ogg");
                    audio::start((stl::string)snd_path.generic_string());

                    sound_started = true;
                }

                constexpr float frequency_hz = 60.0f;
                constexpr float damping_ratio = 1.f;
                b2MouseJointDef jd;

                jd.bodyA = physics::get_ground()->get_body();
                jd.bodyB = current_contol_body->get_body();
                jd.target = mouse_position_absolute;
                jd.maxForce = 1000.0f * current_contol_body->get_body()->GetMass();
                b2LinearStiffness(jd.stiffness, jd.damping, frequency_hz, damping_ratio, jd.bodyA, jd.bodyB);

                current_contol_joint = dynamic_cast<b2MouseJoint*>(physics::get_world().CreateJoint(&jd));
                current_contol_body->get_body()->SetAwake(true);
            }
            else 
            {
                current_contol_joint = nullptr;
            }
        }
        else 
        {
            if (!current_contol_body->is_destroyed())
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
        physics::get_world().DestroyJoint(current_contol_joint);
        current_contol_joint = nullptr;
        current_contol_body = nullptr;
    }
}

void gameplay::holder::free::destroy()
{
}