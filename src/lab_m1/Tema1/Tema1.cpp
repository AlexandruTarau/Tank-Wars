#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <tuple>
#include <iostream>
#include <numbers>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;
using namespace m1;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Tema1::Tema1()
{
    maxVisibleX = 28.f;
    maxVisibleY = 12.f;
    step = 0.05f;
    maxMapX = maxVisibleX * 4;
    visibleChunksNumber = maxVisibleX / step;
    firstChunkIndex = 0;
    lastChunkIndex = maxMapX / step - 1;
    excessChunksNumber = 40;

    gravity = glm::vec3(0.f, 1000.f, 0.f);
    maxHeightDistance = 3.f;
    transferValue = 50.f;

    healthBarHeight = 20.f;
    healthBarWidth = 60.f;

    cameraPosition = glm::vec3(0, 0, 50);
}


Tema1::~Tema1()
{
}


float Tema1::TerrainFunc(float x) {
    return 2.5f +
        1.2f * sin(0.2f * sqrt(x)) +
        1.0f * sin(0.3f * x + 1.5f) +
        0.6f * sin(0.8f * x + 0.5f) +
        0.2f * sin(2.0f * x);
}

glm::vec3 Tema1::ProjectileFunc(glm::vec3 velocity, glm::vec3 gravity, glm::vec3 position, float t)
{
    return position + velocity * t - 0.5f * gravity * t * t;
}

void Tema1::UpdateTank(Tank& tank, float deltaTime)
{
    if (!tank.IsDead()) {
        std::tuple<glm::mat3, glm::mat3> models;
        models = tank.BuildTank(heightMap);

        string name = "";
        if (&tank == &tank1) {
            name = "tank1";
        }
        else if (&tank == &tank2) {
            name = "tank2";
        }
        RenderMesh2D(meshes[name], shaders["VertexColor"], std::get<0>(models));
        RenderMesh2D(meshes["cannon"], shaders["VertexColor"], std::get<1>(models));

        // UI
        // Health Bar
        glm::mat3 modelMatrix = glm::mat3(1);
        glm::vec3 tankPos = tank.GetPosition();
        float tankHeight = Tank::height;

        modelMatrix *= transform2D::Translate(tankPos.x - healthBarWidth / 2.f, tankPos.y + 3.f * tankHeight);
        modelMatrix *= transform2D::Scale(healthBarWidth, healthBarHeight);

        RenderMesh2D(meshes["barBorder"], shaders["VertexColor"], modelMatrix);

        modelMatrix *= transform2D::Scale((float)tank.GetCurrentHealth() / (float)tank.GetMaxHealth(), 1.f);
        RenderMesh2D(meshes["barFill"], shaders["VertexColor"], modelMatrix);

        // Projectile Trajectory
        float cannonAngle = tank.GetCannonAngle();
        float angle = (cannonAngle - 90.f) * glm::pi<float>() / 180.f;
        glm::vec3 cannonOffset = tank.GetCannonOffset();
        float cannonFireOffsetX = -Tank::cannonLength * sin(angle);
        float cannonFireOffsetY = Tank::cannonLength * cos(angle);

        glm::vec3 initialVelocity = glm::vec3(cos(cannonAngle * glm::pi<float>() / 180.f),
            sin(cannonAngle * glm::pi<float>() / 180.f), 0) * Projectile::projectileSpeed;
        glm::vec3 initialPosition = glm::vec3(tankPos.x + cannonOffset.x + cannonFireOffsetX,
            tankPos.y + cannonOffset.y + cannonFireOffsetY, 0.f);

        vector<glm::vec3> trajectoryPoints;

        glm::ivec2 resolution = window->GetResolution();
        float scaleY = (float)resolution.y / maxVisibleY;
        float scaleX = (float)resolution.x / maxVisibleX;

        for (float i = 0.f; i < maxVisibleX; i += step) {

            // Calculate the new position at time t
            glm::vec3 newPosition = ProjectileFunc(initialVelocity, gravity, initialPosition, i / 10);
            
            // Check if the projectile intersects with the terrain
            float ax, ay;
            bool found = false;

            for (int j = 0; j < heightMap.size(); j++) {
                ax = std::get<0>(heightMap[j]);
                ay = std::get<1>(heightMap[j]);
                if ((ax - newPosition.x) * (ax - newPosition.x) + (ay - newPosition.y) * (ay - newPosition.y) < 25.f) {
                    tank.SetIntersection(newPosition);
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }

            // Stop if the projectile would go below the ground level
            if (newPosition.y < 0) {
                break;
            }

            // Store the new point in the trajectory
            trajectoryPoints.push_back(newPosition);

            // Draw line segment between the last position and the new position
            if (trajectoryPoints.size() > 1) {
                glm::mat3 modelMatrix = glm::mat3(1);
                glm::vec3 point1 = trajectoryPoints[trajectoryPoints.size() - 2];
                glm::vec3 point2 = trajectoryPoints[trajectoryPoints.size() - 1];

                float distance = sqrt((point2.y - point1.y) * (point2.y - point1.y) + (point2.x - point1.x) * (point2.x - point1.x));

                float angle = atan2(point2.y - point1.y, point2.x - point1.x);

                modelMatrix *= transform2D::Translate(point1.x, point1.y);
                modelMatrix *= transform2D::Rotate(angle);
                modelMatrix *= transform2D::Scale(distance, 1);

                RenderMesh2D(meshes["line"], shaders["VertexColor"], modelMatrix);
            }
        }
    }
}


void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(cameraPosition);
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    // Initiate Terrain
    float pointScaleX = (float)resolution.x / maxVisibleX;
    float pointScaleY = (float)resolution.y / maxVisibleY;
    for (float x = 0.f; x <= maxMapX; x += step) {
        heightMap.push_back({x * pointScaleX, TerrainFunc(x) * pointScaleY});
    }

    // Initiate Tanks
    int chunkIndex = (int)(1.f / step);
    float posX = std::get<0>(heightMap[chunkIndex]);
    tank1 = Tank(glm::vec3(posX, 0.f, 0.f), chunkIndex, 90.f, 60.f, 100);

    chunkIndex = (int)((maxVisibleX - 1) / step);
    posX = std::get<0>(heightMap[chunkIndex]);
    tank2 = Enemy(glm::vec3(posX, 0.f, 0.f), chunkIndex, 90.f, 60.f, 100, 300, 1.f);

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 1;
    
    Mesh* terrain = object2D::CreateSquare("terrain", corner, squareSide, glm::vec3(0.059, 0.529, 0.075), false);
    Mesh* cannon = object2D::CreateSquare("cannon", corner, squareSide, glm::vec3(0.11, 0.122, 0.11), true);
    Mesh* tank1 = object2D::CreateTank("tank1", corner, Tank::width, Tank::height, Tank::radius, glm::vec3(0.31, 0.188, 0.039), glm::vec3(0.871, 0.208, 0.075), true);
    Mesh* tank2 = object2D::CreateTank("tank2", corner, Tank::width, Tank::height, Tank::radius, glm::vec3(0.31, 0.188, 0.039), glm::vec3(0.071, 0.184, 0.922), true);
    Mesh* circle = object2D::CreateCircle("circle", corner, 1, glm::vec3(0, 0, 0), true);
    Mesh* healthBarBorder = object2D::CreateSquare("barBorder", corner, squareSide, glm::vec3(1, 1, 1), false);
    Mesh* healthBar = object2D::CreateSquare("barFill", corner, squareSide, glm::vec3(1, 1, 1), true);
    Mesh* line = object2D::CreateLine("line");
    
    AddMeshToList(terrain);
    AddMeshToList(cannon);
    AddMeshToList(tank1);
    AddMeshToList(tank2);
    AddMeshToList(circle);
    AddMeshToList(healthBarBorder);
    AddMeshToList(healthBar);
    AddMeshToList(line);
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.11, 0.808, 0.91, 0.651);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema1::Update(float deltaTimeSeconds)
{
    float ax = 0.f, bx = 0.f, ay = 0.f, by = 0.f, t = 0.f;

    // Land slide
    for (int i = max(0, firstChunkIndex - excessChunksNumber); i < min(visibleChunksNumber + firstChunkIndex + excessChunksNumber, lastChunkIndex) - 1; i++) {
        ax = std::get<0>(heightMap[i]);
        bx = std::get<0>(heightMap[i + 1]);
        ay = std::get<1>(heightMap[i]);
        by = std::get<1>(heightMap[i + 1]);

        float heightDif = abs(ay - by);

        if (heightDif > maxHeightDistance) {
            float heightTransfer = std::min(transferValue, heightDif) * deltaTimeSeconds;

            if (ay < by) {
                ay += heightTransfer;
                by -= heightTransfer;
            }
            else {
                ay -= heightTransfer;
                by += heightTransfer;
            }
            std::get<1>(heightMap[i]) = ay;
            std::get<1>(heightMap[i + 1]) = by;
        }
    }

    // Building the map
    for (int i = max(0, firstChunkIndex - excessChunksNumber); i < min(visibleChunksNumber + firstChunkIndex + excessChunksNumber, lastChunkIndex) - 1; i++) {
        modelMatrix = glm::mat3(1);
        ax = std::get<0>(heightMap[i]);
        bx = std::get<0>(heightMap[i + 1]);
        ay = std::get<1>(heightMap[i]);
        by = std::get<1>(heightMap[i + 1]);

        modelMatrix *= transform2D::Translate(ax, ay);
        modelMatrix *= transform2D::Shear((by - ay) / (bx - ax));
        modelMatrix *= transform2D::Scale(bx - ax, max(ay, by));

        RenderMesh2D(meshes["terrain"], shaders["VertexColor"], modelMatrix);
    }

    tank2.UpdateAI(tank1.GetPosition(), heightMap, visibleChunksNumber, firstChunkIndex, gravity, projectiles, projectilesPool, deltaTimeSeconds);

    UpdateTank(tank1, deltaTimeSeconds);
    UpdateTank(tank2, deltaTimeSeconds);

    // Projectiles
    for (auto it = projectiles.begin(); it != projectiles.end(); ) {

        // Projectile rendering
        RenderMesh2D(meshes["circle"], shaders["VertexColor"], it->BuildProjectile(heightMap));

        // Projectile movement
        if (it->Update(heightMap, visibleChunksNumber, firstChunkIndex, lastChunkIndex, excessChunksNumber, tank1, tank2, projectiles, projectilesPool, gravity, window->GetResolution(), deltaTimeSeconds)) {

            // Remove projectile and add it to the pool
            projectilesPool.push_back(*it);
            it = projectiles.erase(it);
        }
        else {
            ++it;
        }
    }
}


void Tema1::FrameEnd()
{
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    // Tank1
    if (window->KeyHold(GLFW_KEY_D)) {
        glm::vec3 position = tank1.GetPosition();
        glm::ivec2 resolution = window->GetResolution();
        int currentChunk = tank1.GetChunkIndex();
        float newPosX = position.x + Tank::speed * deltaTime;
        int i = currentChunk + 1;
        float firstChunkPosX = std::get<0>(heightMap[firstChunkIndex]);
        float lastChunkPosX = std::get<0>(heightMap[lastChunkIndex]);

        if (newPosX > lastChunkPosX - excessChunksNumber) {
            return;
        }

        tank1.SetPosition(glm::vec3(newPosX, position.y, position.z));

        // TODO: try bsearch later
        // Update the chunk the tank is on
        for (; i < visibleChunksNumber + firstChunkIndex; i++) {
            if (std::get<0>(heightMap[i]) > newPosX) {
                tank1.SetChunkIndex(i - 1);
                break;
            }
        }

        if (newPosX > resolution.x / 2.f + firstChunkPosX && newPosX < lastChunkPosX - resolution.x / 2.f) {
            int chunkMoveNumber = i - 1 - currentChunk;

            firstChunkIndex += chunkMoveNumber;
            firstChunkPosX = std::get<0>(heightMap[firstChunkIndex]);

            glm::vec3 targetPos = glm::vec3(firstChunkPosX, 0, 50);
            glm::vec3 difference = targetPos - cameraPosition;

            // Damping camera movement
            cameraPosition += difference * 0.04f;

            GetSceneCamera()->SetPosition(cameraPosition);
        }
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        glm::vec3 position = tank1.GetPosition();
        float newPosX = position.x - Tank::speed * deltaTime;

        if (newPosX < 0) {
            return;
        }

        tank1.SetPosition(glm::vec3(newPosX, position.y, position.z));
        
        // TODO: try bsearch later
        // Update the chunk the tank is on
        for (int i = tank1.GetChunkIndex(); i >= firstChunkIndex; i--) {
            if (std::get<0>(heightMap[i]) <= newPosX) {
                tank1.SetChunkIndex(i);
                break;
            }
        }
    }
    if (window->KeyHold(GLFW_KEY_W)) {
        tank1.UpdateCannonAngle(false, deltaTime);
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        tank1.UpdateCannonAngle(true, deltaTime);
    }

    // Tank2
    if (!tank2.IsActive()) {
        if (window->KeyHold(GLFW_KEY_RIGHT)) {
            glm::vec3 position = tank2.GetPosition();
            float newPosX = position.x + Tank::speed * deltaTime;

            if (newPosX > window->GetResolution().x) {
                return;
            }

            tank2.SetPosition(glm::vec3(newPosX, position.y, position.z));

            // TODO: try bsearch later
            // Update the chunk the tank is on
            for (int i = tank2.GetChunkIndex() + 1; i < visibleChunksNumber + firstChunkIndex; i++) {
                if (std::get<0>(heightMap[i]) > newPosX) {
                    tank2.SetChunkIndex(i - 1);
                    break;
                }
            }
        }
        if (window->KeyHold(GLFW_KEY_LEFT)) {
            glm::vec3 position = tank2.GetPosition();
            float newPosX = position.x - Tank::speed * deltaTime;

            if (newPosX < 0) {
                newPosX = 0;
            }

            tank2.SetPosition(glm::vec3(newPosX, position.y, position.z));

            // TODO: try bsearch later
            // Update the chunk the tank is on
            for (int i = tank2.GetChunkIndex(); i >= firstChunkIndex; i--) {
                if (std::get<0>(heightMap[i]) <= newPosX) {
                    tank2.SetChunkIndex(i);
                    break;
                }
            }
        }
        if (window->KeyHold(GLFW_KEY_UP)) {
            tank2.UpdateCannonAngle(false, deltaTime);
        }
        if (window->KeyHold(GLFW_KEY_DOWN)) {
            tank2.UpdateCannonAngle(true, deltaTime);
        }
    }
}


void Tema1::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_SPACE) {
        tank1.FireProjectile(Projectile::projectileSpeed, gravity, projectiles, projectilesPool);
    }
    if (key == GLFW_KEY_ENTER && !tank2.IsActive()) {
        tank2.FireProjectile(Projectile::projectileSpeed, gravity, projectiles, projectilesPool);
    }
    if (key == GLFW_KEY_I) {
        tank2.SetStatus(!tank2.IsActive());
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
 /*   if (window->KeyHold(GLFW_KEY_A) || window->KeyHold(GLFW_KEY_D)) {
        
    }*/
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
