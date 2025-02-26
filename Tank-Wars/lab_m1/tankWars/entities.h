#pragma once

#include "components/simple_scene.h"

namespace entities {

    class Projectile {
    private:
        glm::vec2 position;
        glm::vec2 velocity;
        bool isActive;

    public:
        Projectile(const glm::vec2& position, const glm::vec2& velocity);

        glm::vec2 GetPosition() const;
        void SetPosition(const glm::vec2& pos);

        glm::vec2 GetVelocity() const;
        void SetVelocity(const glm::vec2& vel);

        bool IsActive() const;
        void SetActive(bool value);

        void Projectile::Update(float deltaTime, float gravity);
        bool CheckCollisionWithTerrain(const std::vector<float>& heightMap);
        bool Projectile::CheckCollisionWithTank(const glm::vec2& tankPosition, float tankRadius);
    };

} // namespace entities
