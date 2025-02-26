#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tankWars/entities.h"
#include <vector>

namespace m1 {

    class TankWars : public gfxc::SimpleScene {
    public:
        TankWars();
        ~TankWars();

        void Init() override;

    private:
        // Overridden methods from SimpleScene
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnWindowResize(int width, int height) override;

        // Custom methods
        void RenderTerrain(int width);
        void CreateMeshes();
        void RenderMeshes();

        void UpdateProjectiles(
            std::vector<entities::Projectile>& activeProjectiles,
            float tankXPos, float& tankHealth, float tankOrigHealth,
            float& healthBarWidth, float healthBarOrigWidth,
            glm::vec3& tankBodyColor, glm::vec3& tankElemColor, float& tankHitTime,
            float opponentXPos, float& opponentHealth, float opponentOrigHealth,
            float& opponentHealthBarWidth, float opponentHealthBarOrigWidth,
            glm::vec3& opponentBodyColor, glm::vec3& opponentElemColor, float& opponentHitTime,
            bool tankInFrame, bool opponentInFrame, float deltaTime);

        void RenderTank(float& tankXPos, float& tankHealth, float& healthBarWidth, float healthBarOrigWidth,
            float& angle, float turretAngle, float& projectileAngle, glm::vec2& turretEndPos, Mesh* tankMesh, Mesh* turretMesh,
            Mesh* outerBarMesh, Mesh* innerBarMesh, bool& tankInFrame, const std::vector<float>& heightMap);

        std::vector<glm::vec2> CalculateTrajectory(glm::vec2 startPosition, float angle, float initialSpeed);
        void RenderTrajectory(const std::vector<glm::vec2>& trajectoryPoints, std::string meshName);
        void DeformTerrain(int impactX, float radius);
        void ApplyLandslide(int impactX, float radius);
        void RenderProjectiles(std::vector<entities::Projectile>& activeProjectiles, std::string projectileName);
        void GenerateHeightMap(int width);

    protected:
        // Model matrices
        glm::mat3 modelMatrix;

        // Meshes and map data
        std::vector<Mesh*> meshList;
        std::vector<float> heightMap;
        std::vector<glm::vec2> starPositions;

        // Timers
        float TankHitTime = 0.0f;
        const float colorChangeDuration = 0.1f;

        // Tank 1 Data
        float tank1XPos = 450;
        float tank1Angle;
        float turret1Angle = 0.0f;
        float tank1ProjectileAngle = 0.0f;
        glm::vec2 turret1EndPos;

        float tank1Health = 400;
        const float tank1OrigHealth = tank1Health;
        bool tank1InFrame = true;

        glm::vec3 origTank1BodyColor = glm::vec3(0.60f, 0.40f, 0.30f);
        glm::vec3 origTank1TurretColor = glm::vec3(0.36f, 0.25f, 0.20f);
        glm::vec3 tank1BodyColor = origTank1BodyColor;
        glm::vec3 tank1TurretColor = origTank1TurretColor;

        std::vector<entities::Projectile> activeProjectiles1;

        float healthBar1Width = 14;
        const float healthBar1OrigWidth = healthBar1Width;
        const float healthBar1Height = 0.2f;

        // Tank 2 Data
        float tank2XPos = 800;
        float tank2Angle;
        float turret2Angle = 0.0f;
        float tank2ProjectileAngle = 0.0f;
        float turret_length;
        glm::vec2 turret2EndPos;

        float tank2Health = 400;
        const float tank2OrigHealth = tank2Health;
        bool tank2InFrame = true;

        glm::vec3 origTank2BodyColor = glm::vec3(0.45f, 0.85f, 0.45f);
        glm::vec3 origTank2TurretColor = glm::vec3(0.13f, 0.55f, 0.13f);
        glm::vec3 tank2BodyColor = origTank2BodyColor;
        glm::vec3 tank2TurretColor = origTank2TurretColor;

        std::vector<entities::Projectile> activeProjectiles2;

        float healthBar2Width = 14;
        const float healthBar2OriginalWidth = healthBar2Width;
        const float healthBar2Height = 0.2f;

        // General settings
        float initialMagnitude = 600.0f;
        float gravity = 600.0f;
    };

} // namespace m1
