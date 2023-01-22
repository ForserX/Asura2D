#include "pch.h"

using namespace Asura::Systems;

void SceneSystem::Init()
{
}

void SceneSystem::Reset()
{
}

void SceneSystem::Tick(float dt)
{
    Entities::AccessView([]()
    {
        auto view = Entities::GetView<Entities::physics_body_component, Entities::scene_component>();

        view.each([](const entt::entity ent, Entities::physics_body_component& phys_comp, Entities::scene_component& scene_comp) 
        {
            if (Entities::IsValid(ent)) 
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
