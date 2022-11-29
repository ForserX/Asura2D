#pragma once

namespace asura::systems
{
    class physics_scene_system final : public system
    {
    public:
        void init() override;
        void reset() override;
        void tick(float dt) override;
    };
}
