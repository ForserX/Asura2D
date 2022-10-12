#include "pch.h"

using namespace ark::systems;

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
        auto view = entities::get_view<
            entities::physics_body_component,
            entities::scene_component
        >();
        
        view.each([](const entt::entity ent, entities::physics_body_component& phys_comp, entities::scene_component& scene_comp) {
            if (entities::is_valid(ent)) {
                if (phys_comp.body != nullptr) {
                    scene_comp.transform.set_position(phys_comp.body->get_position());
                }
            }
        });
    });
}
