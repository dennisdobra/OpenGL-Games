#include "lab_m1/DronesGame/DronesGame.h"
#include "lab_m1/DronesGame/object3D.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


DronesGame::DronesGame()
{
}

DronesGame::~DronesGame()
{
}

/* Terrain function from VertexShader */
float random(const glm::vec2& st) {
    return glm::fract(glm::sin(st.x * 2.0f) + cos(st.y * 4.0f));
}

float noise(const glm::vec2& st) {
    glm::vec2 i = glm::floor(st);
    glm::vec2 f = glm::fract(st);

    float a = random(i);
    float b = random(i + glm::vec2(1.0f, 0.0f));
    float c = random(i + glm::vec2(0.0f, 1.0f));
    float d = random(i + glm::vec2(1.0f, 1.0f));

    glm::vec2 u = f * f * (3.0f - 2.0f * f);
    return glm::mix(a, b, u.x) + (c - a) * u.y * (1.0f - u.x) + (d - b) * u.x * u.y;
}

float getTerrainHeight(float x, float z) {
    glm::vec2 scaledPosition = glm::vec2(x, z) * 0.07f; // Match scale in Vertex Shader
    return noise(scaledPosition) * 30.0f;               // Match amplitude in Vertex Shader (height of hills)
}

void DronesGame::Init() {
    /* Set the cameras */
    camera = new implemented::DroneCamera();
    //camera->Set(glm::vec3(0, 2, 5.0f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    camera->Set(glm::vec3(84.0f, 35.0f, 79.0f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    minimapCamera = new implemented::DroneCamera();
    minimapCamera->Set(glm::vec3(0, 100, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));

    /* Set up the minimap viewport area */
    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(50, 50, resolution.x / 4.f, resolution.y / 4.f);

    /* Set how far I can see */
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 6000.0f);

    srand((unsigned int)time(NULL));    

    /* RANDOM POSITIONS FOR TREES */
    float minDistanceTrees = 3.0f;
    int maxAttempts = 50;
    const int totalTrees = 1500;
    for (int i = 0; i < totalTrees; ++i) {
        glm::vec3 position;
        int attempts = 0;
        bool validPosition = false;

        while (attempts < maxAttempts) {
            attempts++;

            float x = static_cast<int>(rand() % 290 - 145);
            float z = static_cast<int>(rand() % 290 - 145);
            float y = getTerrainHeight(x, z);
            position = glm::vec3(x, y, z);

            // Check the distance from all existing trees
            validPosition = true;
            for (const auto& existingPosition : treePositions) {
                if (glm::distance(position, existingPosition) < minDistanceTrees) {
                    validPosition = false;
                    break;
                }
            }

            if (validPosition) {
                break;
            }
        }

        if (validPosition) {
            treePositions.push_back(position);

            // Assign a tree type based on the index
            if (i < totalTrees / 3) {
                treeTypes.push_back("treeOlivine");
            }
            else if (i < 2 * totalTrees / 3) {
                treeTypes.push_back("treeRussian");
            }
            else {
                treeTypes.push_back("treeOlive");
            }
        }
    }

    /* RANDOM POSITIONS FOR THE WIND MILLS */
    float minDistanceMills = 55.0f;
    maxAttempts = 20;
    const int totalWindMills = 15;
    for (int i = 0; i < totalWindMills; i++) {
        glm::vec3 position;
        float angle;
        int attempts = 0;
        bool validPosition = false;

        while (attempts < maxAttempts) {
            attempts++;

            float x = static_cast<int>(rand() % 290 - 145); // Random x [-150, 150]
            float z = static_cast<int>(rand() % 290 - 145); // Random z [-150, 150]
            float y = getTerrainHeight(x, z);
            position = glm::vec3(x, y, z);

            angle = static_cast<float>(rand() % 360);

            // check distance from all the existring mills
            validPosition = true;
            for (const auto& existingPosition : millPositions) {
                if (glm::distance(position, existingPosition) < minDistanceMills) {
                    validPosition = false;
                    break;
                }
            }

            // check distance from all existing trees
            for (const auto& existingPosition : treePositions) {
                if (glm::distance(position, existingPosition) < minDistanceTrees) {
                    validPosition = false;
                    break;
                }
            }

            if (validPosition) {
                break;
            }
        }

        if (validPosition) {
            millPositions.push_back(position);
            millAngles.push_back(angle);
        }
    }

    /* RANDOM POSITIONS FOR STARS */
    const int totalStars = 2000;
    for (int i = 0; i < totalStars; i++) {
        glm::vec3 position;
        float x = static_cast<float>(rand() % 8000 - 4000);
        float z = static_cast<float>(rand() % 8000 - 4000);
        float y = static_cast<float>(rand() % 100 + 200);

        position = glm::vec3(x, y, z);
        starPositions.push_back(position);
    }

    /* RANDOM POSITIONS FOR PACKET PICKUPS */
    float minDistancePacket = 5.0f;
    maxAttempts = 20;
    const int totalPositions = 5;
    for (int i = 0; i < totalPositions; i++) {
        glm::vec3 position;
        int attempts = 0;
        bool validPosition = false;

        while (attempts < maxAttempts) {
            attempts++;

            float x = static_cast<int>(rand() % 200 - 100);
            float z = static_cast<int>(rand() % 200 - 100);
            float y = 0.16f + getTerrainHeight(x, z);
            position = glm::vec3(x, y, z);

            validPosition = true;

            // check distance from all existing mills
            for (const auto& existingPosition : millPositions) {
                if (glm::distance(position, existingPosition) < minDistancePacket) {
                    validPosition = false;
                    break;
                }
            }

            // check distance from all existing trees
            for (const auto& existingPosition : treePositions) {
                if (glm::distance(position, existingPosition) < minDistancePacket) {
                    validPosition = false;
                    break;
                }
            }

            if (validPosition) {
                break;
            }
        }
        
        if (validPosition) {
            packetPickUps.push_back(position);
        }
        else {
            cout << "Could not load PickUp number " << i << endl;
        }
    }

    /* RANDOM POSITIONS FOR PACKET DROPOFF */
    for (int i = 0; i < totalPositions; i++) {
        glm::vec3 position;
        int attempts = 0;
        bool validPosition = false;

        while (attempts < maxAttempts) {
            attempts++;

            float x = static_cast<int>(rand() % 200 - 100);
            float z = static_cast<int>(rand() % 200 - 100);
            float y = 0.16f + getTerrainHeight(x, z);
            position = glm::vec3(x, y, z);

            validPosition = true;

            // check distance from all existing mills
            for (const auto& existingPosition : millPositions) {
                if (glm::distance(position, existingPosition) < minDistancePacket) {
                    validPosition = false;
                    break;
                }
            }

            // check distance from all existing trees
            for (const auto& existingPosition : treePositions) {
                if (glm::distance(position, existingPosition) < minDistancePacket) {
                    validPosition = false;
                    break;
                }
            }

            if (validPosition) {
                break;
            }
        }

        if (validPosition) {
            packetDropOffs.push_back(position);
        }
        else {
            cout << "Could not load DropOff number " << i << endl;
        }
    }

    /* TERRAIN SHADER */
    Shader* terrainShader = new Shader("TerrainShader");
    terrainShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "DronesGame", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
    terrainShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "DronesGame", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
    terrainShader->CreateAndLink();
    shaders[terrainShader->GetName()] = terrainShader;

    CreateMeshes();
    CreateTerrain();

    cout << endl << endl << "============================================================" << endl << endl;
    cout << "\t\t\tSTART GAME!" << endl << endl;
    cout << "    - Your task is to deliver all packets successfully -" << endl << endl;
}

void DronesGame::CreateTerrain() {
    const int width = 300;
    const int depth = 300;

    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    /* Generate vertices (squares on the map) */
    for (int z = 0; z <= depth; ++z) {
        for (int x = 0; x <= width; ++x) {
            float height = 0.0f; // height of each point
            vertices.emplace_back(
                glm::vec3((float)x - width / 2.0f, height, (float)z - depth / 2.0f) // 3D position
            );
        }
    }

    /* Generate indices (divide each square in two traingles) */
    for (int z = 0; z < depth; ++z) {
        for (int x = 0; x < width; ++x) {
            int start = z * (width + 1) + x;

            // first triangle
            indices.push_back(start);
            indices.push_back(start + 1);
            indices.push_back(start + width + 1);

            // Second triangle
            indices.push_back(start + 1);
            indices.push_back(start + width + 2);
            indices.push_back(start + width + 1);
        }
    }

    Mesh* terrainMesh = new Mesh("terrain");
    terrainMesh->InitFromData(vertices, indices);
    meshes[terrainMesh->GetMeshID()] = terrainMesh;
}

void DronesGame::CreateMeshes()
{
    /* Create the sky: sun, moon and stars */
    {
        Mesh* sun = object3dimensions::CreateSphere(
            "sun",
            glm::vec3(0.0f, 0.0f, 0.0f),      // center
            10.0f,                            // Radius
            glm::vec3(1.0f, 0.7f, 0.2f),      // Yellow color
            36,                               // sectors
            18                                // stacks
        );
        meshes[sun->GetMeshID()] = sun;

        Mesh* moon = object3dimensions::CreateSphere(
            "moon",
            glm::vec3(0.0f, 0.0f, 0.0f),
            6.0f,
            glm::vec3(0.94f, 0.92f, 0.82f),
            30,
            14
        );
        meshes[moon->GetMeshID()] = moon;

        Mesh* star = object3dimensions::CreateSphere(
            "star",
            glm::vec3(0.0f, 0.0f, 0.0f),
            0.5f,
            glm::vec3(1.0f, 1.0f, 1.0f),
            10,
            5
        );
        meshes[star->GetMeshID()] = star;
    }
    
    /* Create the drone */
    {
        float width, height, depth;

        /* DRONE BODY */
        width = 1.3f, height = 0.1f, depth = 0.1f;
        Mesh* parallelepiped1 = object3dimensions::CreateParallelipiped(
            "parallelipiped1", glm::vec3(0, 0, 0), width, height, depth, glm::vec3(1.0f, 0.5f, 0.0f));
        meshes[parallelepiped1->GetMeshID()] = parallelepiped1;

        width = 0.1f, height = 0.1f, depth = 1.3f;
        Mesh* parallelepiped2 = object3dimensions::CreateParallelipiped(
            "parallelipiped2", glm::vec3(0, 0, 0), width, height, depth, glm::vec3(1.0f, 0.5f, 0.0f));
        meshes[parallelepiped2->GetMeshID()] = parallelepiped2;

        /* DRONE CUBES */
        width = 0.1f, height = 0.1f / 2.0f, depth = 0.1f;

        Mesh* cube1 = object3dimensions::CreateParallelipiped(
            "cube1", glm::vec3(0, 0, 0), width, height, depth, glm::vec3(0.9f, 0.45f, 0.0f));
        meshes[cube1->GetMeshID()] = cube1;

        Mesh* cube2 = object3dimensions::CreateParallelipiped(
            "cube2", glm::vec3(0, 0, 0), width, height, depth, glm::vec3(0.9f, 0.45f, 0.0f));
        meshes[cube2->GetMeshID()] = cube2;

        Mesh* cube3 = object3dimensions::CreateParallelipiped(
            "cube3", glm::vec3(0, 0, 0), width, height, depth, glm::vec3(0.9f, 0.45f, 0.0f));
        meshes[cube3->GetMeshID()] = cube3;

        Mesh* cube4 = object3dimensions::CreateParallelipiped(
            "cube4", glm::vec3(0, 0, 0), width, height, depth, glm::vec3(0.9f, 0.45f, 0.0f));
        meshes[cube4->GetMeshID()] = cube4;

        /* DRONE PROPELLERS */
        width = 0.4f, height = 0.01f, depth = 0.03f;

        Mesh* propeller1 = object3dimensions::CreateParallelipiped(
            "propeller1", glm::vec3(0, 0, 0), width, height, depth, glm::vec3(1.0f, 0.08f, 0.58f));
        meshes[propeller1->GetMeshID()] = propeller1;

        Mesh* propeller2 = object3dimensions::CreateParallelipiped(
            "propeller2", glm::vec3(0, 0, 0), width, height, depth, glm::vec3(1.0f, 0.08f, 0.58f));
        meshes[propeller2->GetMeshID()] = propeller2;

        Mesh* propeller3 = object3dimensions::CreateParallelipiped(
            "propeller3", glm::vec3(0, 0, 0), width, height, depth, glm::vec3(1.0f, 0.08f, 0.58f));
        meshes[propeller3->GetMeshID()] = propeller3;

        Mesh* propeller4 = object3dimensions::CreateParallelipiped(
            "propeller4", glm::vec3(0, 0, 0), width, height, depth, glm::vec3(1.0f, 0.08f, 0.58f));
        meshes[propeller4->GetMeshID()] = propeller4;
    }
    
    /* Create the tree */
    {
        glm::vec3 trunkColor = glm::vec3(0.365f, 0.227f, 0.102f);
        glm::vec3 olivineColor = glm::vec3(0.604f, 0.725f, 0.451f);
        glm::vec3 russianGreen = glm::vec3(0.404f, 0.573f, 0.404f);
        glm::vec3 oliveGreen = glm::vec3(0.502f, 0.502f, 0.0f);

        // tree 1
        Mesh* treeOlivine = object3dimensions::CreateTreeMesh(
            "treeOlivine",
            glm::vec3(0, 0, 0),                 // Base center
            0.3f, 7.0f,                         // Trunk radius and height
            1.5f, 2.5f,                         // Foliage base radius and height
            glm::vec3(0.396f, 0.263f, 0.129f),  // Trunk color
            glm::vec3(0.604f, 0.725f, 0.451f),  // Foliage color
            30                                 // Sectors
        );
        meshes[treeOlivine->GetMeshID()] = treeOlivine;

        // tree 2
        Mesh* treeRussian = object3dimensions::CreateTreeMesh(
            "treeRussian",
            glm::vec3(0, 0, 0),
            0.3f, 7.0f,
            1.5f, 2.5f,
            glm::vec3(0.302f, 0.200f, 0.098f),
            glm::vec3(0.404f, 0.573f, 0.404f),
            30
        );
        meshes[treeRussian->GetMeshID()] = treeRussian;

        // tree 3
        Mesh* treeOlive = object3dimensions::CreateTreeMesh(
            "treeOlive",
            glm::vec3(0, 0, 0),
            0.3f, 7.0f,
            1.5f, 2.5f,
            glm::vec3(0.365f, 0.227f, 0.102f),
            glm::vec3(0.502f, 0.502f, 0.0f),
            30
        );
        meshes[treeOlive->GetMeshID()] = treeOlive;
    }
    
    /* Create the wind mill  */
    {
        Mesh* cylinder = object3dimensions::CreateCylinder(
            "cylinder",
            glm::vec3(0, 0, 0),
            0.8f, 25.0f,                    // radius, height
            glm::vec3(1.0f, 1.0f, 1.0f),
            50
        );
        meshes[cylinder->GetMeshID()] = cylinder;

        Mesh* cone = object3dimensions::CreateCone(
            "cone",
            glm::vec3(0, 0, 0),
            0.8, 1.0,
            glm::vec3(1.0f, 1.0f, 1.0f),
            50
        );
        meshes[cone->GetMeshID()] = cone;

        Mesh* windPropeller1 = object3dimensions::CreatePropeller(
            "windPropeller1",
            glm::vec3(0, 0, 0),
            1, 14, 0.5,
            glm::vec3(0.95f, 0.95f, 0.92f)
        );
        meshes[windPropeller1->GetMeshID()] = windPropeller1;

        Mesh* windPropeller2 = object3dimensions::CreatePropeller(
            "windPropeller2",
            glm::vec3(0, 0, 0),
            1, 14, 0.5,
            glm::vec3(0.95f, 0.95f, 0.92f)
        );
        meshes[windPropeller2->GetMeshID()] = windPropeller2;

        Mesh* windPropeller3 = object3dimensions::CreatePropeller(
            "windPropeller3",
            glm::vec3(0, 0, 0),
            1, 14, 0.5,
            glm::vec3(0.95f, 0.95f, 0.92f)
        );
        meshes[windPropeller3->GetMeshID()] = windPropeller3;
    }
    
    /* Create the pick up packet + pick up sign */
    {
        Mesh* box = object3dimensions::CreateParallelipiped(
            "box",
            glm::vec3(0, 0, 0),
            0.7f, 0.7f, 0.7f,
            glm::vec3(0.745f, 0.145f, 0.518f)
        );
        meshes[box->GetMeshID()] = box;

        Mesh* boxLid = object3dimensions::CreateParallelipiped(
            "boxLid",
            glm::vec3(0, 0, 0),
            0.8f, 0.1f, 0.8f,
            glm::vec3(0.298f, 0.058f, 0.207f)
        );
        meshes[boxLid->GetMeshID()] = boxLid;

        Mesh* pickUp = object3dimensions::CreateParallelipiped(
            "pickUp",
            glm::vec3(0, 0, 0),
            0.2f, 60.0f, 0.2f,
            glm::vec3(1.0f, 0.6f, 0.0f)
        );
        meshes[pickUp->GetMeshID()] = pickUp;
    }

    /* Create the delivered packet */
    {
        Mesh* deliveredBox = object3dimensions::CreateParallelipiped(
            "deliveredBox",
            glm::vec3(0, 0, 0),
            0.7f, 0.7f, 0.7f,
            glm::vec3(0.45f, 0.85f, 0.45f)
        );
        meshes[deliveredBox->GetMeshID()] = deliveredBox;

        Mesh* deliveredLid = object3dimensions::CreateParallelipiped(
            "deliveredLid",
            glm::vec3(0, 0, 0),
            0.8f, 0.1f, 0.8f,
            glm::vec3(0.13f, 0.55f, 0.13f)
        );
        meshes[deliveredLid->GetMeshID()] = deliveredLid;

        Mesh* dropOff = object3dimensions::CreateParallelipiped(
            "dropOff",
            glm::vec3(0, 0, 0),
            0.2f, 60.0f, 0.2f,
            glm::vec3(1.0f, 0.08f, 0.58f)
        );
        meshes[dropOff->GetMeshID()] = dropOff;
    }
}

void DronesGame::RenderScene(float deltaTimeSeconds, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    glm::mat4 modelMatrix;

    /* Render the trees */
    {
        for (size_t i = 0; i < treePositions.size(); i++) {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, treePositions[i]);
            RenderMesh(meshes[treeTypes[i]], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);
        }
    }

    /* Render the wind mills */
    {
        for (size_t i = 0; i < millPositions.size(); i++) {
            glm::vec3 currentPosition = millPositions[i];
            float Xcoordinate = currentPosition.x;
            float Zcoordinate = currentPosition.z;
            float Ycoordinate = currentPosition.y - 0.5f;
            float angle = millAngles[i];

            /* BODY */
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 12.5, 0));
            modelMatrix = glm::translate(modelMatrix, glm::vec3(Xcoordinate, Ycoordinate, Zcoordinate));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
            RenderMesh(meshes["cylinder"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);

            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 25.5, 0)); // lift the cone with the height of the cylinder
            modelMatrix = glm::translate(modelMatrix, glm::vec3(Xcoordinate, Ycoordinate, Zcoordinate));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
            RenderMesh(meshes["cone"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);

            /* PROPELLERS */
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(Xcoordinate, Ycoordinate, Zcoordinate));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0, 1, 0));  // Rotate the entire windmill
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 24, 0.8));                 // Position the propeller
            modelMatrix = glm::rotate(modelMatrix, glm::radians(windPropellerAngle), glm::vec3(0, 0, 1)); // Rotate around Z-axis
            RenderMesh(meshes["windPropeller1"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);

            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(Xcoordinate, Ycoordinate, Zcoordinate)); // Translate to random position
            modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0, 1, 0));  // Rotate the entire windmill
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 24, 0.8));                 // Position the propeller
            modelMatrix = glm::rotate(modelMatrix, RADIANS(120), glm::vec3(0, 0, 1));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(windPropellerAngle), glm::vec3(0, 0, 1)); // Rotate around Z-axis
            RenderMesh(meshes["windPropeller2"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);

            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(Xcoordinate, Ycoordinate, Zcoordinate)); // Translate to random position
            modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0, 1, 0));  // Rotate the entire windmill
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 24, 0.8));                 // Position the propeller
            modelMatrix = glm::rotate(modelMatrix, RADIANS(240), glm::vec3(0, 0, 1));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(windPropellerAngle), glm::vec3(0, 0, 1)); // Rotate around Z-axis
            RenderMesh(meshes["windPropeller3"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);
        }
    }

    /* Render the sky */
    {
        if (!isNight) {
            /* Render the sun */
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-105.0f, 125.0f, -110.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(4.0f));
            RenderMesh(meshes["sun"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);
        }
        else if (isNight) {
            /* Render the moon + stars */
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-105.0f, 125.0f, -110.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(4.0f));
            RenderMesh(meshes["moon"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);

            for (size_t i = 0; i < starPositions.size(); i++) {
                glm::vec3 currentPosition = starPositions[i];

                modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(currentPosition.x, currentPosition.y, currentPosition.z));
                RenderMesh(meshes["star"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);
            }
        }
    }
    

    dronePosition = camera->GetTargetPosition();
    glm::vec3 localForward = glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z));
    glm::vec3 localRight = glm::normalize(glm::vec3(camera->right.x, 0, camera->right.z));
    glm::vec3 localUp = glm::normalize(glm::cross(localRight, localForward));

    float droneAngle = atan2(localForward.x, localForward.z);
    float angle = glm::radians(45.0f);

    /* Render the drone */
    {
        // Render the first parallelepiped
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, dronePosition);
        modelMatrix = glm::rotate(modelMatrix, currentTiltAngleX, localRight);
        modelMatrix = glm::rotate(modelMatrix, currentTiltAngleZ, localForward);
        modelMatrix = glm::rotate(modelMatrix, angle + droneAngle, glm::vec3(0, 1, 0));
        RenderMesh(meshes["parallelipiped1"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);

        // Render the second parallelepiped
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, dronePosition);
        modelMatrix = glm::rotate(modelMatrix, currentTiltAngleX, localRight);
        modelMatrix = glm::rotate(modelMatrix, currentTiltAngleZ, localForward);
        modelMatrix = glm::rotate(modelMatrix, angle + droneAngle, glm::vec3(0, 1, 0));
        RenderMesh(meshes["parallelipiped2"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);

        // Render the cubes
        float width1 = 1.3f, height1 = 0.1f, depth1 = 0.1f;
        float cubeOffset = 0.1f / 2.0f;
        glm::vec3 cubePositions[] = {
            glm::vec3(width1 / 2 - cubeOffset, height1 / 2.0f + cubeOffset / 2.0f, 0),
            glm::vec3(-width1 / 2 + cubeOffset, height1 / 2.0f + cubeOffset / 2.0f, 0),
            glm::vec3(0, height1 / 2.0f + cubeOffset / 2.0f, -(width1 / 2 - cubeOffset)),
            glm::vec3(0, height1 / 2.0f + cubeOffset / 2.0f, width1 / 2 - cubeOffset)
        };
        std::string cubeNames[] = { "cube1", "cube2", "cube3", "cube4" };

        for (int i = 0; i < 4; i++) {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, dronePosition);
            modelMatrix = glm::rotate(modelMatrix, currentTiltAngleX, localRight);
            modelMatrix = glm::rotate(modelMatrix, currentTiltAngleZ, localForward);
            modelMatrix = glm::rotate(modelMatrix, angle + droneAngle, glm::vec3(0, 1, 0));
            modelMatrix = glm::translate(modelMatrix, cubePositions[i]);
            RenderMesh(meshes[cubeNames[i]], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);
        }

        // Render the propellers
        float heightP = 0.01f;
        glm::vec3 propellerOffsets[] = {
            glm::vec3(width1 / 2 - cubeOffset, height1, 0),
            glm::vec3(-width1 / 2 + cubeOffset, height1, 0),
            glm::vec3(0, height1, -(width1 / 2 - cubeOffset)),
            glm::vec3(0, height1, width1 / 2 - cubeOffset)
        };
        std::string propellerNames[] = { "propeller1", "propeller2", "propeller3", "propeller4" };

        rotationAngle += deltaTimeSeconds * propellerSpeed;

        for (int i = 0; i < 4; i++) {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, dronePosition);
            modelMatrix = glm::rotate(modelMatrix, currentTiltAngleX, localRight); // Left/Right tilt
            modelMatrix = glm::rotate(modelMatrix, currentTiltAngleZ, localForward); // Forward/Backward tilt
            modelMatrix = glm::rotate(modelMatrix, angle + droneAngle, glm::vec3(0, 1, 0)); // Align with camera
            modelMatrix = glm::translate(modelMatrix, propellerOffsets[i]);
            if (i == 0 || i == 1) {
                modelMatrix = glm::rotate(modelMatrix, rotationAngle, glm::vec3(0, 1, 0));
            }
            else {
                modelMatrix = glm::rotate(modelMatrix, -rotationAngle, glm::vec3(0, 1, 0));
            }
            RenderMesh(meshes[propellerNames[i]], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);
        }
    }
    

    /* Render current packet at pick up point or under the drone */
    if (packetAttached) {
        modelMatrix = glm::mat4(1);

        modelMatrix = glm::translate(modelMatrix, dronePosition); // Get the drone's position
        modelMatrix = glm::rotate(modelMatrix, currentTiltAngleX, localRight);  // Apply tilt on OX
        modelMatrix = glm::rotate(modelMatrix, currentTiltAngleZ, localForward); // Apply tilt on OZ

        // Align the packet under the drone + match drone's rotation
        glm::vec3 adjustedPacketOffset = glm::vec3(0.0f, -0.5f, 0.0f);
        modelMatrix = glm::translate(modelMatrix, adjustedPacketOffset);
        modelMatrix = glm::rotate(modelMatrix, angle + droneAngle, glm::vec3(0, 1, 0));

        // Render the box
        RenderMesh(meshes["box"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);

        // Render the lid
        glm::mat4 lidMatrix = modelMatrix; // use same matrix as for the box
        glm::vec3 boxLidOffset = glm::vec3(0.0f, 0.4f, 0.0f);
        lidMatrix = glm::translate(lidMatrix, boxLidOffset);
        RenderMesh(meshes["boxLid"], shaders["VertexColor"], lidMatrix, viewMatrix, projectionMatrix);
    }
    else {
        // Render unpicked packet
        if (currentPickUpIndex < packetPickUps.size()) {
            glm::vec3 packetPosition = packetPickUps[currentPickUpIndex];

            // Render the main box
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, packetPosition);
            RenderMesh(meshes["box"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);

            // Render the lid slightly above the box
            glm::vec3 boxLidOffset = glm::vec3(0.0f, 0.4f, 0.0f);
            modelMatrix = glm::translate(modelMatrix, boxLidOffset);
            RenderMesh(meshes["boxLid"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);
        }
    }

    /* Render pick up sign */
    {
        if (renderPickUp && currentPickUpIndex < packetPickUps.size()) {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 29.5f, 0.0f));
            modelMatrix = glm::translate(modelMatrix, packetPickUps[currentPickUpIndex]);
            RenderMesh(meshes["pickUp"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);
        }
    }

    /* Render drop off sign */
    {
        if (renderDropOff && currentDropOffIndex < packetDropOffs.size()) {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 29.5f, 0.0f));
            modelMatrix = glm::translate(modelMatrix, packetDropOffs[currentDropOffIndex]);
            RenderMesh(meshes["dropOff"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);
        }
    }

    /* Render delivered packets */
    {
        for (const auto& position : deliveredPacketPositions) {
            // Render the delivered box
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, position);
            RenderMesh(meshes["deliveredBox"], shaders["VertexColor"], modelMatrix, viewMatrix, projectionMatrix);

            // Render the lid slightly above the delivered box
            glm::mat4 lidMatrix = modelMatrix;
            glm::vec3 boxLidOffset = glm::vec3(0.0f, 0.4f, 0.0f);
            lidMatrix = glm::translate(lidMatrix, boxLidOffset);
            RenderMesh(meshes["deliveredLid"], shaders["VertexColor"], lidMatrix, viewMatrix, projectionMatrix);
        }
    }

    /* Render the terrain */
    {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -0.1, 0));
        RenderMesh(meshes["terrain"], shaders["TerrainShader"], modelMatrix, viewMatrix, projectionMatrix);
    }

}

void DronesGame::FrameStart()
{
    if (!isNight) {
        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    }
    else if (isNight) {
        //glClearColor(0.050f, 0.015f, 0.120f, 1.0f);
        glClearColor(0.030f, 0.010f, 0.070f, 1.0f);


    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

bool DronesGame::CollisionWithTree()
{
    float verticalCollisionHeight = packetAttached ? 9.0f : 8.0f;
    float treeCollisionRadius;

    if (dronePosition.y <= getTerrainHeight(dronePosition.x, dronePosition.z) + 3.5) {
        treeCollisionRadius = 0.6f; // collision with trunk
    }
    else if (dronePosition.y > getTerrainHeight(dronePosition.x, dronePosition.z) + 3.5) {
        treeCollisionRadius = 1.6; // collision with foliage
    }

    for (const glm::vec3& treePosition : treePositions) {
        // Check the horizontal distance between the drone and the tree
        float horizontalDistance = glm::distance(glm::vec2(dronePosition.x, dronePosition.z), glm::vec2(treePosition.x, treePosition.z));

        // Check the vertical distance (height difference) between the drone and the tree
        float verticalDistance = std::abs(dronePosition.y - treePosition.y);

        if (horizontalDistance < treeCollisionRadius && verticalDistance < verticalCollisionHeight) {
            return true;
        }
    }

    return false;
}

bool DronesGame::CollisionWithMill() {
    float verticalCollsionHeight = 25.5f;
    float millCollisionRadius = 1.5f;

    for (const glm::vec3& millPosition : millPositions) {
        float horizontalDistance = glm::distance(glm::vec2(dronePosition.x, dronePosition.z), glm::vec2(millPosition.x, millPosition.z));
        float verticalDistance = std::abs(dronePosition.y - millPosition.y);

        if (horizontalDistance < millCollisionRadius && verticalDistance < verticalCollsionHeight) {
            return true;
        }
    }

    return false;
}

void DronesGame::Update(float deltaTimeSeconds) {
    windPropellerAngle += deltaTimeSeconds * 25.0f;
    if (windPropellerAngle > 360.0f) {
        windPropellerAngle -= 360.0f;
    }

    if (CollisionWithTree()) {
        treeCollisionDetected = true;
    }
    if (CollisionWithMill()) {
        millCollisionDetected = true;
    }

    /* Create the MiniMap */
    {
        // Clear the depth buffer for the minimap
        glClear(GL_DEPTH_BUFFER_BIT);

        // Set the viewport for the minimap
        glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);

        // Use an orthographic projection for the minimap
        float mapSize = 20.0f;
        glm::mat4 minimapProjection = glm::ortho(-mapSize, mapSize, -mapSize, mapSize, 0.1f, 1000.0f);

        glm::vec3 dronePos = camera->GetTargetPosition();

        // Position the minimap camera directly above the map center
        glm::vec3 minimapPosition = glm::vec3(dronePos.x, 50.0f, dronePos.z + 15.0f);
        glm::vec3 minimapTarget = glm::vec3(dronePos.x, 0.0f, dronePos.z);

        // Build a static top-down view matrix
        glm::mat4 minimapViewMatrix = glm::lookAt(
            minimapPosition,
            minimapTarget,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // Render the scene using the minimap camera
        RenderScene(deltaTimeSeconds, minimapViewMatrix, minimapProjection);

        // Reset the viewport to cover the full screen (for main camera)
        glm::ivec2 resolution = window->GetResolution();
        glViewport(0, 0, resolution.x, resolution.y);
    }
    
    /* Packet pick up/drop off logic */
    if (renderPickUp && !packetAttached) {
        glm::vec3 pickUpPosition = packetPickUps[currentPickUpIndex];
        float distanceToPickUp = glm::distance(dronePosition, pickUpPosition);

        if (distanceToPickUp < 2.0f) {
            packetAttached = true;
            renderPickUp = false;
            renderDropOff = true;
            cout << "Packet number " << currentPickUpIndex << " picked up from position: (x = " << pickUpPosition.x << ", y = " << pickUpPosition.y << ", z = " << pickUpPosition.z << ")" << endl;
        }
    }

    if (renderDropOff && packetAttached) {
        if (currentDropOffIndex < packetDropOffs.size()) {
            glm::vec3 dropOffPosition = packetDropOffs[currentDropOffIndex];
            float distanceToDropOff = glm::distance(dronePosition, dropOffPosition);

            if (distanceToDropOff < 2.0f) {
                packetAttached = false;
                renderDropOff = false;
                currentPickUpIndex++;

                // Add the drop-off position to the list of delivered packets
                deliveredPacketPositions.push_back(dropOffPosition);

                cout << "Packet number " << currentDropOffIndex << " droped off at position: (x = " << dropOffPosition.x << ", y = " << dropOffPosition.y << ", z = " << dropOffPosition.z << ")" << endl << endl;
                currentDropOffIndex++;

                if (currentPickUpIndex < packetPickUps.size()) {
                    renderPickUp = true;
                }
                else {
                    cout << "    - You won! All packets were delivered successfully -" << endl << endl;
                    cout << endl << endl << "============================================================" << endl << endl;
                    renderPickUp = false;
                    renderDropOff = false;
                }
            }
        }
    }

    // Render the main scene
    RenderScene(deltaTimeSeconds, camera->GetViewMatrix(), projectionMatrix);
}

void DronesGame::FrameEnd()
{
    
}

void DronesGame::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Use the specified shader
    shader->Use();

    // Set the uniform variables for the view, projection, and model matrices
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Render the mesh
    mesh->Render();
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see input_controller.h.
 */

void DronesGame::OnInputUpdate(float deltaTime, int mods)
{
    // Speeds and constants
    float cameraSpeed = 20.0f;
    float droneSpeed = 50.0f;
    float cameraRotationSpeed = 0.5f;
    propellerSpeed = 7.0f;
    droneTiltAngleX = 0.0f;
    droneTiltAngleZ = 0.0f;
    bool isMoving = false;
    float gravity = 4.0f;

    // Define the minimum allowed altitude based on packet attachment
    float altitude = getTerrainHeight(dronePosition.x, dronePosition.z);
    float minAltitude = packetAttached ? 0.7f + altitude : 0.05f + altitude;

    // Handle collision logic
    if (treeCollisionDetected || millCollisionDetected) {
        if (dronePosition.y > minAltitude) {
            droneVerticalVelocity -= gravity * deltaTime;
            dronePosition.y = glm::max(dronePosition.y + droneVerticalVelocity * deltaTime, minAltitude);
            camera->TranslateUpward(glm::max(droneVerticalVelocity * deltaTime, minAltitude - dronePosition.y));
            camera->MoveForward(-cameraSpeed / 15.0f * deltaTime);

            if (dronePosition.y <= minAltitude + 0.2f) {
                treeCollisionDetected = false;
                millCollisionDetected = false;
            }
        }
        return;
    }

    // Adjust speed when the drone is close to the ground
    if (dronePosition.y <= minAltitude + 0.1f) {
        cameraSpeed *= 0.05f;
        droneSpeed *= 0.05f;
    }

    // Turbo mode
    if (window->KeyHold(GLFW_KEY_SPACE)) {
        cameraSpeed *= 2.0f;
        droneSpeed *= 5.0f;
        propellerSpeed *= 5.0f;
    }

    // Slow mode
    if (window->KeyHold(GLFW_KEY_RIGHT_SHIFT)) {
        cameraSpeed /= 8.0f;
        propellerSpeed /= 2.0f;
    }

    // Handle forward and backward movement
    if (window->KeyHold(GLFW_KEY_W)) {
        isMoving = true;
        droneTiltAngleX = (dronePosition.y >= minAltitude + 0.1f) ? glm::radians(-25.0f) : 0.0f; // Forward tilt (does not tilt when it s on the ground)
        camera->MoveForward(cameraSpeed * deltaTime);
        dronePosition += glm::vec3(camera->forward.x, 0, camera->forward.z) * deltaTime;
    }
    else if (window->KeyHold(GLFW_KEY_S)) {
        isMoving = true;
        droneTiltAngleX = (dronePosition.y >= minAltitude + 0.1f) ? glm::radians(25.0f) : 0.0f; // Backward tilt
        camera->MoveForward(-cameraSpeed * deltaTime);
        dronePosition += glm::vec3(camera->forward.x, 0, camera->forward.z) * deltaTime;
    }

    // Handle left and right movement
    if (window->KeyHold(GLFW_KEY_A)) {
        isMoving = true;
        droneTiltAngleZ = (dronePosition.y >= minAltitude + 0.1f) ? glm::radians(-25.0f) : 0.0f; // Left tilt
        camera->TranslateRight(-cameraSpeed * deltaTime);
        dronePosition += glm::vec3(camera->right.x, 0, camera->right.z) * deltaTime;
    }
    else if (window->KeyHold(GLFW_KEY_D)) {
        isMoving = true;
        droneTiltAngleZ = (dronePosition.y >= minAltitude + 0.1f) ? glm::radians(25.0f) : 0.0f; // Right tilt
        camera->TranslateRight(cameraSpeed * deltaTime);
        dronePosition += glm::vec3(camera->right.x, 0, camera->right.z) * deltaTime;
    }

    // Smooth tilt transitions
    currentTiltAngleX += (droneTiltAngleX - currentTiltAngleX) * deltaTime * tiltSpeed;
    currentTiltAngleZ += (droneTiltAngleZ - currentTiltAngleZ) * deltaTime * tiltSpeed;

    // Handle vertical movement
    if (window->KeyHold(GLFW_KEY_E)) { // Move upwards
        droneVerticalVelocity = 4.0f;
        dronePosition.y += droneVerticalVelocity * deltaTime;
        camera->TranslateUpward(cameraSpeed * deltaTime);
    }
    else if (window->KeyHold(GLFW_KEY_Q)) { // Move downwards
        if (dronePosition.y > minAltitude) {
            droneVerticalVelocity = gravityEnabled ? droneVerticalVelocity - (gravity * deltaTime) : -droneSpeed / 10.0f;
            dronePosition.y = glm::clamp(dronePosition.y + droneVerticalVelocity * deltaTime, minAltitude, FLT_MAX);
            camera->TranslateUpward(droneVerticalVelocity * deltaTime);

            if (dronePosition.y <= minAltitude) {
                dronePosition.y = minAltitude;
                droneVerticalVelocity = 0.0f;
            }
        }
    }

    // Apply gravity when no vertical movement keys are pressed
    if (gravityEnabled && dronePosition.y > minAltitude) {
        droneVerticalVelocity -= gravity * deltaTime;
        dronePosition.y = glm::max(dronePosition.y + droneVerticalVelocity * deltaTime, minAltitude);
        camera->TranslateUpward(glm::max(droneVerticalVelocity * deltaTime, minAltitude - dronePosition.y));

        if (dronePosition.y <= minAltitude) {
            dronePosition.y = minAltitude;
            droneVerticalVelocity = 0.0f;
        }
    }

    // Final altitude clamping
    if (dronePosition.y < minAltitude) {
        dronePosition.y = minAltitude;
        droneVerticalVelocity = 0.0f;
    }

    HandleTurningAndCameraRotation(cameraRotationSpeed, deltaTime);

    // Stop propellers when drone is on the ground
    if (dronePosition.y == minAltitude && !isMoving) {
        propellerSpeed = 0.0f;
    }
}

void DronesGame::HandleTurningAndCameraRotation(float cameraRotationSpeed, float deltaTime) {
    // Handle turning with key combinations
    if (window->KeyHold(GLFW_KEY_W) && window->KeyHold(GLFW_KEY_A)) {
        camera->RotateThirdPerson_OY(cameraRotationSpeed * deltaTime);
    }
    else if (window->KeyHold(GLFW_KEY_W) && window->KeyHold(GLFW_KEY_D)) {
        camera->RotateThirdPerson_OY(-cameraRotationSpeed * deltaTime);
    }
    else if (window->KeyHold(GLFW_KEY_S) && window->KeyHold(GLFW_KEY_A)) {
        camera->RotateThirdPerson_OY(-cameraRotationSpeed * deltaTime);
    }
    else if (window->KeyHold(GLFW_KEY_S) && window->KeyHold(GLFW_KEY_D)) {
        camera->RotateThirdPerson_OY(cameraRotationSpeed * deltaTime);
    }

    // Handle camera rotation
    if (window->KeyHold(GLFW_KEY_LEFT)) {
        camera->RotateThirdPerson_OY(cameraRotationSpeed * deltaTime);
    }
    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        camera->RotateThirdPerson_OY(-cameraRotationSpeed * deltaTime);
    }
    if (window->KeyHold(GLFW_KEY_UP)) {
        camera->RotateThirdPerson_OX(-cameraRotationSpeed * deltaTime);
    }
    if (window->KeyHold(GLFW_KEY_DOWN)) {
        camera->RotateThirdPerson_OX(cameraRotationSpeed * deltaTime);
    }
}

void DronesGame::OnKeyPress(int key, int mods)
{
    // enable and disable gravity
    if (key == GLFW_KEY_ENTER) {
        gravityEnabled = !gravityEnabled;
        droneVerticalVelocity = 2.0f;
    }

    // day and night mode
    if (key == GLFW_KEY_N) {
        isNight = !isNight;
    }
}

void DronesGame::OnKeyRelease(int key, int mods)
{
    // Add key release event
}

void DronesGame::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}

void DronesGame::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}

void DronesGame::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}

void DronesGame::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void DronesGame::OnWindowResize(int width, int height)
{
    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(50, 50, resolution.x / 4.f, resolution.y / 4.f);
}