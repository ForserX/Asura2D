#include "pch.h"

using namespace asura::systems;

void
physics_scene_system::init()
{
}

void
physics_scene_system::reset()
{
}

void
physics_scene_system::tick(float dt)
{
    entities::access_view([](){
        auto view = entities::get_view<entities::physics_body_component, entities::scene_component>();
        view.each([](const entt::entity ent, entities::physics_body_component& phys_comp, entities::scene_component& scene_comp) {
            if (entities::is_valid(ent)) {
                if (phys_comp.body != nullptr) {
                    const auto rect = phys_comp.body->get_rect();
                    scene_comp.size = math::fvec2(rect.width(), rect.height());
                    scene_comp.transform.set_position(phys_comp.body->get_position());
                    scene_comp.transform.set_angle(phys_comp.body->get_angle());
                }
            }
        });
    });
}
