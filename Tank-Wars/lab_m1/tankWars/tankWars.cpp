#include "lab_m1/tankWars/tankWars.h"
#include "lab_m1/tankWars/transform2D.h"
#include "lab_m1/tankWars/object2D.h"
#include "lab_m1/tankWars/entities.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;

TankWars::TankWars()
{
    activeProjectiles1.clear();
    activeProjectiles2.clear();
}

TankWars::~TankWars()
{
}

void TankWars::DeformTerrain(int impactX, float radius) {
    glm::ivec2 resolution = window->GetResolution();

    // Check if impactX is within the screen bounds
    if (impactX < 0 || impactX >= resolution.x) {
        return;
    }

    // Store the original height at impactX for reference
    float originalHeight = heightMap[impactX];

    // Define the interval of x-values affected by the deformation
    int startX = std::max(0, impactX - static_cast<int>(radius));
    int endX = std::min(resolution.x - 1, impactX + static_cast<int>(radius));

    // Traverse the heightMap within the deformation radius
    for (int x = startX; x <= endX; ++x) {
        float distance = static_cast<float>(std::abs(x - impactX));

        // Check if the point is within the circular area of the radius
        if (distance <= radius) {
            // depth of deformation based on the distance from the impact point
            // (creates a semicircular deformation)
            float deformationDepth = sqrt(radius * radius - distance * distance);

            // Apply deformation -> using std::min() ensures the terrain is only lowered and never raised
            heightMap[x] = std::min(heightMap[x], originalHeight - deformationDepth);
        }
    }
}

void TankWars::ApplyLandslide(int impactX, float radius) {
    int start = std::max(1, impactX - static_cast<int>(radius));
    int end = std::min(static_cast<int>(heightMap.size()) - 2, impactX + static_cast<int>(radius));

    // calculate average of neighboring values multiple times for realistic effect
    for (int iteration = 0; iteration < 125; iteration++) {
        for (int i = start; i <= end; i++) {
            heightMap[i] = (heightMap[i - 1] + heightMap[i + 1]) / 2.0f;
        }
    }
}

std::vector<glm::vec2> TankWars::CalculateTrajectory(glm::vec2 startPosition, float angle, float initialSpeed) {
    glm::ivec2 resolution = window->GetResolution();

    const float timeStep = 0.05f;    // small value for a smoother trajectory
    const int maxSteps = 200;        // max number of points in the trajectory

    std::vector<glm::vec2> trajectoryPoints;
    glm::vec2 position = startPosition;
    glm::vec2 velocity(
        cos(angle) * initialSpeed,
        sin(angle) * initialSpeed
    );

    for (int i = 0; i < maxSteps; i++) {
        // Save the current position as part of the trajectory
        trajectoryPoints.push_back(position);

        // Update position based on velocity and time step
        position += velocity * timeStep;
        velocity.y -= gravity * timeStep;

        // check if the trajectory is out of bounds
        int xPos = static_cast<int>(position.x);
        int yPos = static_cast<int>(position.y);
        if (xPos < 0 || xPos >= resolution.x ||
            yPos < heightMap[xPos] - 20 || yPos < 0) {
            break;
        }
    }

    return trajectoryPoints;
}

void TankWars::RenderTrajectory(const std::vector<glm::vec2>& trajectoryPoints, std::string meshName) {
    float segmentThickness = 0.05f;     // Thickness of each line segment (dot)
    float segmentLengthScale = 0.75f;   // continuous line for trajectory 
    //float segmentLengthScale = 0.1f;  // discontinuous line for trajectory

    for (size_t i = 0; i < trajectoryPoints.size() - 1; i++) {
        glm::vec2 start = trajectoryPoints[i];
        glm::vec2 end = trajectoryPoints[i + 1];

        // Calculate midpoint and shortened length for each segment
        glm::vec2 midPoint = (start + end) / 2.0f;
        float length = glm::distance(start, end) * segmentLengthScale;
        float angle = atan2(end.y - start.y, end.x - start.x);

        // Transform and render each segment
        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(midPoint.x, midPoint.y);
        modelMatrix *= transform2D::Rotate(angle);
        modelMatrix *= transform2D::Scale(length, segmentThickness);

        RenderMesh2D(meshes[meshName], shaders["VertexColor"], modelMatrix);
    }
}

void TankWars::UpdateProjectiles(
    std::vector<entities::Projectile>& activeProjectiles,
    float selfXPos, float& selfHealth, float selfOrigHealth,
    float& selfHealthBarWidth, float selfHealthBarOrigWidth,
    glm::vec3& selfBodyColor, glm::vec3& selfElemColor, float& selfHitTime,
    float opponentXPos, float& opponentHealth, float opponentOrigHealth,
    float& opponentHealthBarWidth, float opponentHealthBarOrigWidth,
    glm::vec3& opponentBodyColor, glm::vec3& opponentElemColor, float& opponentHitTime,
    bool selfInFrame, bool opponentInFrame, float deltaTime)
{
    const float tankCollisionRadius = 47.0f;
     
    for (auto& projectile : activeProjectiles) {
        if (projectile.IsActive()) {
            // Update the projectile's position and velocity
            projectile.Update(deltaTime, gravity);

            // Check collision with terrain
            if (projectile.CheckCollisionWithTerrain(heightMap)) {
                int impactX = static_cast<int>(projectile.GetPosition().x);
                if (impactX >= 0 && impactX < static_cast<int>(heightMap.size())) {
                    DeformTerrain(impactX, 20.0f);
                    ApplyLandslide(impactX, 40.0f);
                }
                continue;
            }

            // Check collision with self tank
            if (selfInFrame && projectile.CheckCollisionWithTank(glm::vec2(selfXPos, heightMap[static_cast<int>(selfXPos)]), tankCollisionRadius)) {
                selfHealth -= 10;
                projectile.SetActive(false);

                // Update self health bar
                float healthPercentage = selfHealth / selfOrigHealth;
                selfHealthBarWidth = selfHealthBarOrigWidth * healthPercentage;

                // Change color to indicate impact
                selfBodyColor = glm::vec3(0.8f, 0.3f, 0.3f);
                selfElemColor = glm::vec3(0.8f, 0.3f, 0.3f);
                selfHitTime = glfwGetTime();
                continue;
            }

            // Check collision with opponent tank
            if (opponentInFrame && projectile.CheckCollisionWithTank(glm::vec2(opponentXPos, heightMap[static_cast<int>(opponentXPos)]), tankCollisionRadius)) {
                opponentHealth -= 10;
                projectile.SetActive(false);

                // Update opponent health bar
                float healthPercentage = opponentHealth / opponentOrigHealth;
                opponentHealthBarWidth = opponentHealthBarOrigWidth * healthPercentage;

                // Change color to indicate impact
                opponentBodyColor = glm::vec3(0.8f, 0.3f, 0.3f);
                opponentElemColor = glm::vec3(0.8f, 0.3f, 0.3f);
                opponentHitTime = glfwGetTime();
                continue;
            }
        }
    }

    // Remove inactive projectiles
    activeProjectiles.erase(
        std::remove_if(activeProjectiles.begin(), activeProjectiles.end(),
            [](const entities::Projectile& p) { return !p.IsActive(); }),
        activeProjectiles.end()
    );
}

void TankWars::CreateMeshes()
{
    /* Crewate SQUARE FOR MAP */
    float side_length = 1.0f;
    glm::vec3 corner(0, -1, 0);
    glm::vec3 terrainColor(0.85f, 0.7f, 0.5f); // beige
    Mesh* square = object2dimensions::CreateSquare("square", corner, side_length, terrainColor, true);
    AddMeshToList(square);

    // turret sizes && projectile sizes && trajectory size
    float turret_width = 5.0f;
    float turret_height = 40.0f;
    turret_length = turret_height;
    glm::vec3 projectile_center(0.0f, 0.0f, 0.0f);
    float projectile_radius = 5.0f;
    float segmentWidth = 0.1f;  // Width of each line segment in the trajectory

    /* Create TANK1 */
    Mesh* tank1 = object2dimensions::CreateTank("tank1", tank1TurretColor, tank1BodyColor);
    AddMeshToList(tank1);
    Mesh* turret1 = object2dimensions::CreateRectangle("turret1", tank1TurretColor, true);
    AddMeshToList(turret1);
    Mesh* projectile1 = object2dimensions::CreateCircle("projectile1", projectile_center, projectile_radius, origTank1BodyColor);
    AddMeshToList(projectile1);
    Mesh* outerBar1 = object2dimensions::CreateRectangle("outerBar1", glm::vec3(1.0f, 1.0f, 1.0f), false);
    AddMeshToList(outerBar1);
    Mesh* innerBar1 = object2dimensions::CreateRectangle("innerBar1", origTank1BodyColor, true);
    AddMeshToList(innerBar1);
    Mesh* trajectorySegment1 = object2dimensions::CreateRectangle("trajectorySegment1", origTank1TurretColor, true);
    AddMeshToList(trajectorySegment1);

    /* Create TANK2 */
    Mesh* tank2 = object2dimensions::CreateTank("tank2", tank2TurretColor, tank2BodyColor);
    AddMeshToList(tank2);
    Mesh* turret2 = object2dimensions::CreateRectangle("turret2", tank2TurretColor, true);
    AddMeshToList(turret2);
    Mesh* projectile2 = object2dimensions::CreateCircle("projectile2", projectile_center, projectile_radius, origTank2BodyColor);
    AddMeshToList(projectile2);
    Mesh* outerBar2 = object2dimensions::CreateRectangle("outerBar2", glm::vec3(1.0f, 1.0f, 1.0f), false);
    AddMeshToList(outerBar2);
    Mesh* innerBar2 = object2dimensions::CreateRectangle("innerBar2", origTank2BodyColor, true);
    AddMeshToList(innerBar2);Mesh* trajectorySegment2 = object2dimensions::CreateRectangle("trajectorySegment2", origTank2TurretColor, true);
    AddMeshToList(trajectorySegment2);

    /* Create MOON */
    glm::vec3 moon_center(0, 0, 0);
    float moon_radius = 100;
    glm::vec3 moon_color(0.94f, 0.92f, 0.82f);
    Mesh* moon = object2dimensions::CreateCircle("moon", moon_center, moon_radius, moon_color);
    AddMeshToList(moon);

    /* Create STARS */
    float star_radius = 1.0f;
    for (int i = 0; i < 150; i++) {
        float x = static_cast<float>(rand() % window->GetResolution().x);
        float y = static_cast<float>(rand() % window->GetResolution().y);
        starPositions.push_back(glm::vec2(x, y));

        glm::vec3 starColor(1.0f, 1.0f, 1.0f);
        Mesh* star = object2dimensions::CreateCircle("star" + std::to_string(i), glm::vec3(x, y, 0), star_radius, starColor);
        AddMeshToList(star);
    }
}

void TankWars::RenderTank(float& tankXPos, float& tankHealth, float& healthBarWidth, float healthBarOrigWidth,
    float& angle, float turretAngle, float& projectileAngle, glm::vec2& turretEndPos, Mesh* tankMesh, Mesh* turretMesh,
    Mesh* outerBarMesh, Mesh* innerBarMesh, bool& tankInFrame, const std::vector<float>& heightMap)
{
    /* Destroy terrain under tank when it dies */
    if (tankHealth <= 0 || tankXPos <= 0 || tankXPos >= window->GetResolution().x) {
        tankInFrame = false;
        return;
    }

    /* SLOPE AND POSITION ON MAP */
    float slope = heightMap[tankXPos + 1] - heightMap[tankXPos];
    angle = atan2(slope, 1.0f);
    float tankYPos = heightMap[tankXPos];

    /* TANK BODY */
    glm::mat3 modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(tankXPos, tankYPos) * transform2D::Rotate(angle);
    RenderMesh2D(tankMesh, shaders["VertexColor"], modelMatrix);

    /* TURRET */
    float turretLift = 17.0f;
    modelMatrix *= transform2D::Translate(0, turretLift) * transform2D::Rotate(turretAngle);
    RenderMesh2D(turretMesh, shaders["VertexColor"], modelMatrix);

    /* CALCULATE TURRET END COORDINATES FOR PROJECTILES AND TRAJECTORY */
    glm::vec2 turretBasePos = glm::vec2(
        tankXPos + cos(angle + RADIANS(90)) * turretLift,
        tankYPos + sin(angle + RADIANS(90)) * turretLift
    );

    turretEndPos = glm::vec2(
        turretBasePos.x + cos(angle + turretAngle + RADIANS(90.0f)) * turret_length,
        turretBasePos.y + sin(angle + turretAngle + RADIANS(90.0f)) * turret_length
    );

    /* CALCULATE PROJECTILE ANGLE */
    projectileAngle = angle + turretAngle + RADIANS(90.0f);

    /* HEALTH BAR */
    float healthBarHeight = 50.0f;
    glm::mat3 healthBarOuterMatrix = glm::mat3(1);
    healthBarOuterMatrix *= transform2D::Translate(tankXPos, tankYPos) *
        transform2D::Translate(0, healthBarHeight) *
        transform2D::Scale(healthBarOrigWidth, healthBar1Height);
    RenderMesh2D(outerBarMesh, shaders["VertexColor"], healthBarOuterMatrix);

    glm::mat3 healthBarInnerMatrix = glm::mat3(1);
    healthBarInnerMatrix *= transform2D::Translate(tankXPos, tankYPos) *
        transform2D::Translate(-2 * (healthBarOrigWidth - healthBarWidth), healthBarHeight) *
        transform2D::Scale(healthBarWidth, healthBar1Height);
    RenderMesh2D(innerBarMesh, shaders["VertexColor"], healthBarInnerMatrix);
}

void TankWars::RenderProjectiles(std::vector<entities::Projectile>& activeProjectiles, std::string projectileName) {
    for (auto& projectile : activeProjectiles) {
        if (projectile.IsActive()) {
            glm::vec2 projectilePosition = projectile.GetPosition();

            glm::mat3 projectileModelMatrix = glm::mat3(1);
            projectileModelMatrix *= transform2D::Translate(projectilePosition.x, projectilePosition.y);
            RenderMesh2D(meshes[projectileName], shaders["VertexColor"], projectileModelMatrix);
        }
    }
}

void TankWars::RenderMeshes()
{
    /* Render Tank1 */
    RenderTank(tank1XPos, tank1Health,
        healthBar1Width, healthBar1OrigWidth, tank1Angle, turret1Angle, tank1ProjectileAngle, turret1EndPos,
        object2dimensions::CreateTank("tank1", tank1TurretColor, tank1BodyColor),
        object2dimensions::CreateRectangle("turret1", tank1TurretColor, true),
        meshes["outerBar1"], meshes["innerBar1"], tank1InFrame, heightMap);
    
    /* Render Tank2 */
    RenderTank(tank2XPos, tank2Health,
        healthBar2Width, healthBar2OriginalWidth,
        tank2Angle, turret2Angle, tank2ProjectileAngle, turret2EndPos,
        object2dimensions::CreateTank("tank2", tank2TurretColor, tank2BodyColor),
        object2dimensions::CreateRectangle("turret2", tank2TurretColor, true),
        meshes["outerBar2"], meshes["innerBar2"], tank2InFrame, heightMap);

    /* Render Terrain */
    RenderTerrain(heightMap.size() - 1);

    /* Render Active Projectiles for Tank1 and Tank2 (flying projectiles) */
    RenderProjectiles(activeProjectiles1, "projectile1");
    RenderProjectiles(activeProjectiles2, "projectile2");

    /* Render Trajectory for Tank1 */
    if (tank1Health > 0 && tank1InFrame) {
        std::vector<glm::vec2> trajectoryTank1 = CalculateTrajectory(turret1EndPos, tank1Angle + turret1Angle + RADIANS(90.0f), initialMagnitude);
        RenderTrajectory(trajectoryTank1, "trajectorySegment1");
    }

    /* Render Trajectory for Tank2 */
    if (tank2Health > 0 && tank2InFrame) {
        std::vector<glm::vec2> trajectoryTank2 = CalculateTrajectory(turret2EndPos, tank2Angle + turret2Angle + RADIANS(90.0f), initialMagnitude);
        RenderTrajectory(trajectoryTank2, "trajectorySegment2");
    }

    /* Render the Moon */
    glm::mat3 modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(150, 550);
    RenderMesh2D(meshes["moon"], shaders["VertexColor"], modelMatrix);

    /* Render the Stars */
    for (int i = 0; i < starPositions.size(); i++) {
        modelMatrix = glm::mat3(1);
        RenderMesh2D(meshes["star" + std::to_string(i)], shaders["VertexColor"], modelMatrix);
    }
}

void TankWars::GenerateHeightMap(int width) {
    heightMap.clear();
    for (int x = 0; x < width; x++) {
        float y = 250 + (40.0f * sin(0.005f * x) +
            30.0f * cos(0.015f * x + 1.5f) +
            25.0f * sin(0.03f * x + 0.7f) +
            15.0f * cos(0.07f * x) +
            10.0f * sin(0.01f * x + 2.3f)) * 0.5f;

        heightMap.push_back(y);
    }
}

void TankWars::RenderTerrain(int width)
{
    // Render the map terrain
    for (size_t i = 0; i < width; i++) {
        float xA = i;
        float yA = heightMap[i];
        float xB = i + 1;
        float yB = heightMap[i + 1];

        glm::mat3 scaleMatrix = transform2D::Scale(xB - xA, std::max(yB, yA));
        glm::mat3 shearMatrix = transform2D::Shear((yB - yA) / (xB - xA));
        glm::mat3 translateMatrix = transform2D::Translate(xA, yA);

        glm::mat3 modelMatrix = translateMatrix * shearMatrix * scaleMatrix;

        RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);
    }
}

void TankWars::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);
    // DO NOT MODIFY THE CODE ABOVE

    // Generate the height map vector
    GenerateHeightMap(resolution.x + 10);

    CreateMeshes();
}

void TankWars::FrameStart()
{
    glClearColor(0.063f, 0.02f, 0.141f, 1.0f); // deep purple (darker)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void TankWars::Update(float deltaTimeSeconds)
{
    // check if tanks are in frame
    tank1InFrame = (tank1XPos > 0 && tank1XPos < window->GetResolution().x) ? true : false;
    tank2InFrame = (tank2XPos > 0 && tank2XPos < window->GetResolution().x) ? true : false;

    RenderMeshes();

    UpdateProjectiles(activeProjectiles1,
        tank1XPos, tank1Health, tank1OrigHealth,
        healthBar1Width, healthBar1OrigWidth,
        tank1BodyColor, tank1TurretColor, TankHitTime,
        tank2XPos, tank2Health, tank2OrigHealth,
        healthBar2Width, healthBar2OriginalWidth,
        tank2BodyColor, tank2TurretColor, TankHitTime,
        tank1InFrame, tank2InFrame, deltaTimeSeconds);

    UpdateProjectiles(activeProjectiles2,
        tank2XPos, tank2Health, tank2OrigHealth,
        healthBar2Width, healthBar2OriginalWidth,
        tank2BodyColor, tank2TurretColor, TankHitTime,
        tank1XPos, tank1Health, tank1OrigHealth,
        healthBar1Width, healthBar1OrigWidth,
        tank1BodyColor, tank1TurretColor, TankHitTime,
        tank2InFrame, tank1InFrame, deltaTimeSeconds);

    /* Changing color for tanks */
    float currentTime = glfwGetTime();

    // Restore tank1 color if duration has passed
    if (currentTime - TankHitTime >= colorChangeDuration) {
        tank1BodyColor = origTank1BodyColor;
        tank1TurretColor = origTank1TurretColor;
    }

    // Restore tank2 color if duration has passed
    if (currentTime - TankHitTime >= colorChangeDuration) {
        tank2BodyColor = origTank2BodyColor;
        tank2TurretColor = origTank2TurretColor;
    }
}

void TankWars::FrameEnd()
{
}

void TankWars::OnInputUpdate(float deltaTime, int mods)
{
    float tankSpeed = 150.0f * deltaTime;
    float turretSpeed = deltaTime;

    // Tank 1 controls
    if (window->KeyHold(GLFW_KEY_A)) {
        tank1XPos -= tankSpeed;
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        tank1XPos += tankSpeed;
    }
    if (window->KeyHold(GLFW_KEY_W) && tank1InFrame) {
        turret1Angle += turretSpeed;
    }
    if (window->KeyHold(GLFW_KEY_S) && tank1InFrame) {
        turret1Angle -= turretSpeed;
    }

    // Tank 2 controls
    if (window->KeyHold(GLFW_KEY_LEFT)) {
        tank2XPos -= tankSpeed;
    }
    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        tank2XPos += tankSpeed;
    }
    if (window->KeyHold(GLFW_KEY_UP) && tank2InFrame) {
        turret2Angle += turretSpeed;
    }
    if (window->KeyHold(GLFW_KEY_DOWN) && tank2InFrame) {
        turret2Angle -= turretSpeed;
    }
}

void TankWars::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_SPACE && tank1Health > 0 && tank1InFrame) {
        // vector defining the direction and speed of the projectile when launched
        glm::vec2 initialVelocity = glm::vec2(
            cos(tank1ProjectileAngle) * initialMagnitude,
            sin(tank1ProjectileAngle) * initialMagnitude
        );

        // Create a new instance of the projectile starting at the tip of the turret
        entities::Projectile newProjectile(turret1EndPos, initialVelocity);

        // Add the new projectile to the active projectiles list
        activeProjectiles1.push_back(newProjectile);
    }

    if (key == GLFW_KEY_ENTER && tank2Health > 0 && tank2InFrame) {
        glm::vec2 initialVelocity = glm::vec2(
            cos(tank2ProjectileAngle) * initialMagnitude,
            sin(tank2ProjectileAngle) * initialMagnitude
        );

        entities::Projectile newProjectile(turret2EndPos, initialVelocity);
        activeProjectiles2.push_back(newProjectile);
    }
}

void TankWars::OnWindowResize(int width, int height)
{
    // Regenerate the height map with the new width
    glm::ivec2 resolution = window->GetResolution();
    GenerateHeightMap(resolution.x + 10);

    // Adjust camera projection to match the new resolution
    //auto camera = GetSceneCamera();
    //camera->SetOrthographic(0, static_cast<float>(resolution.x), 0, static_cast<float>(resolution.y), 0.01f, 400);
    //camera->Update();

    // Adjust tank positions to ensure they remain within the new screen bounds
    //tank1XPos = glm::clamp(tank1XPos, 0.0f, static_cast<float>(resolution.x - 1));
    //tank2XPos = glm::clamp(tank2XPos, 0.0f, static_cast<float>(resolution.x - 1));
}
