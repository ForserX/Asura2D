#pragma once

namespace Asura::Systems
{
    class physics_scene_system final : public system
    {
    public:
        void Init() override;
        void Reset() override;
        void Tick(float dt) override;
    };
}
