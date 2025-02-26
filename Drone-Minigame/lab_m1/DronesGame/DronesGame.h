#pragma once

#include "components/simple_scene.h"
#include "lab_m1/DronesGame/camera.h"


namespace m1
{
    class DronesGame : public gfxc::SimpleScene
    {
     public:
         struct ViewportArea
         {
             ViewportArea() : x(0), y(0), width(1), height(1) {}
             ViewportArea(int x, int y, int width, int height)
                 : x(x), y(y), width(width), height(height) {
             }
             int x;
             int y;
             int width;
             int height;
         };

         DronesGame();
        ~DronesGame();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        //void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;
        void DronesGame::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        // my functions
        void DronesGame::CreateMeshes();
        void DronesGame::RenderScene(float deltaTimeSeconds, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
        bool DronesGame::CollisionWithTree();
        bool DronesGame::CollisionWithMill();
        void DronesGame::HandleTurningAndCameraRotation(float cameraRotationSpeed, float deltaTime);
        void DronesGame::CreateTerrain();

     protected:
        implemented::DroneCamera *camera;
        implemented::DroneCamera* minimapCamera;
        ViewportArea miniViewportArea;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        // TODO(student): If you need any other class variables, define them here.
        glm::vec3 dronePosition = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraOffset = glm::vec3(0.0f, 2.0f, 5.0f);
        float rotationAngle = 0.0f;
        float propellerSpeed;
        float droneTiltAngleX = 0.0f;
        float droneTiltAngleZ = 0.0f;
        float windPropellerAngle = 0.0f;

        float droneVerticalVelocity = 0.0f;

        bool gravityEnabled = true;
        bool isNight = false;

        std::vector<glm::vec3> treePositions;
        std::vector<glm::vec3> millPositions;
        std::vector<glm::vec3> starPositions;
        std::vector<float> millAngles;
        std::vector<std::string> treeTypes;

        /* Tilting */
        float currentTiltAngleX = 0.0f;
        float currentTiltAngleZ = 0.0f;
        const float tiltSpeed = 5.0f; // Controls the smoothness of the tilt

        /* Collisions */
        bool treeCollisionDetected = false;
        bool millCollisionDetected = false;
        
        /* Packet logic */
        std::vector<glm::vec3> packetPickUps;
        std::vector<glm::vec3> packetDropOffs;
        std::vector<glm::vec3> deliveredPacketPositions;
        int currentPickUpIndex = 0;   // Tracks the current pick-up point index
        int currentDropOffIndex = 0;  // Tracks the current drop-off point index
        bool packetAttached = false;  // Tracks if the drone has picked up a packet
        bool renderPickUp = true;     // Indicates whether to render the pick-up point
        bool renderDropOff = false;   // Indicates whether to render the drop-off point
    };
}   // namespace m1
