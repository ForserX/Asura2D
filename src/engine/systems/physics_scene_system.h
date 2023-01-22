#pragma once

namespace Asura::Systems
{
    class SceneSystem final : public ISystem
    {
    public:
        void Init() override;
        void Reset() override;
        void Tick(float dt) override;
    };
}
