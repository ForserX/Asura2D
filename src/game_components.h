
/*
 #define DECLARE_SERIALIZABLE_FLAGS \
     entities::background_flag, \
     entities::drawable_flag, \
     entities::ground_flag, \
     entities::level_flag, \
     entities::net_id_flag, \
     entities::net_controlled_flag

 #define DECLARE_SERIALIZABLE_TYPES \
     DECLARE_SERIALIZABLE_FLAGS, \
     entities::draw_color_component, \
     entities::draw_gradient_component, \
     entities::draw_texture_component, \
     entities::scene_component, \
     entities::physics_body_component, \
     entities::dynamic_visual_component

 #define DECLARE_SERIALIZABLE_ENTITY_TYPES \
     DECLARE_SERIALIZABLE_FLAGS, \
     DECLARE_SERIALIZABLE_TYPES

 #define DECLARE_NON_SERIALIZABLE_TYPES \
     entities::garbage_flag, \
     entities::non_serializable_flag, \
     entities::dont_free_after_reset_flag**/

namespace undo
{
    

}

#define DECLARE_GAME_SERIALIZABLE_FLAGS
#define DECLARE_GAME_SERIALIZABLE_TYPES

#define DECLARE_GAME_SERIALIZABLE_ENTITY_TYPES \
    DECLARE_GAME_SERIALIZABLE_FLAGS \
    DECLARE_GAME_SERIALIZABLE_TYPES

#define DECLARE_GAME_NON_SERIALIZABLE_ENTITY_TYPES
