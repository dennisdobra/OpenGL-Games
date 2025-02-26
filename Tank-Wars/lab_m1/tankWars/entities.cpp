#include "entities.h"
#include <glm/glm.hpp>
#include <vector>
#include "transform2D.h"
#include "object2D.h"
#include <unordered_map>

using namespace entities;

// Constructor to initialize the projectile with position and velocity
Projectile::Projectile(const glm::vec2& position, const glm::vec2& velocity)
    : position(position), velocity(velocity), isActive(true) {}

glm::vec2 Projectile::GetPosition() const {
    return position;
}

void Projectile::SetPosition(const glm::vec2& pos) {
    position = pos;
}

glm::vec2 Projectile::GetVelocity() const {
    return velocity;
}

void Projectile::SetVelocity(const glm::vec2& vel) {
    velocity = vel;
}

bool Projectile::IsActive() const {
    return isActive;
}

void Projectile::SetActive(bool value) {
    isActive = value;
}

void Projectile::Update(float deltaTime, float gravity) {
    position += velocity * deltaTime;
    velocity.y -= gravity * deltaTime;

    SetPosition(position);
    SetVelocity(velocity);
}

bool Projectile::CheckCollisionWithTerrain(const std::vector<float>& heightMap) {
    if (position.x >= 0 && position.x < heightMap.size() && position.y >= 10) {
        int xCoord = static_cast<int>(position.x);
        if (position.y <= heightMap[xCoord]) {
            isActive = false;
            return true;
        }
    }
    return false;
}

bool Projectile::CheckCollisionWithTank(const glm::vec2& tankPosition, float tankRadius) {
    // Calculate the distance between the projectile and the tank's position
    float distance = glm::distance(position, tankPosition);

    if (distance <= tankRadius) {
        isActive = false; // Deactivate projectile
        return true;      // Collision occurred
    }

    // No collision
    return false;
}

