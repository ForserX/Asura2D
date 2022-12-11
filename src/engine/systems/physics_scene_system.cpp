#include "pch.h"

using namespace Asura::systems;

void physics_scene_system::Init()
{
}

void physics_scene_system::Reset()
{
}

void physics_scene_system::Tick(float dt)
{
    entities::access_view([]()
    {
        auto view = entities::get_view<entities::physics_body_component, entities::scene_component>();

        view.each([](const entt::entity ent, entities::physics_body_component& phys_comp, entities::scene_component& scene_comp) 
        {
            if (entities::IsValid(ent)) 
            {
                if (phys_comp.body != nullptr) 
                {
                    const auto Rect = phys_comp.body->get_rect();
                    scene_comp.size = Math::FVec2(Rect.width(), Rect.height());
                    scene_comp.Transform.set_position(phys_comp.body->get_position());
                    scene_comp.Transform.set_angle(phys_comp.body->get_angle());
                }
            }
        });
    });
}
