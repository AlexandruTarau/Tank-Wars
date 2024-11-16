#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <tuple>
#include <iostream>
#include <numbers>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2Dmodified.h"

using namespace std;
using namespace m1;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Tema1* Tema1::instance = nullptr;

Tema1::Tema1()
{
    maxVisibleX = 28.f;
    maxVisibleY = 12.f;
    step = 0.1f;
    maxMapX = maxVisibleX * 4;
    visibleChunksNumber = maxVisibleX / step;
    firstChunkIndex1 = 0;
    firstChunkIndex2 = 0;
    lastChunkIndex = maxMapX / step - 1;
    excessChunksNumber = 40;

    gravity = glm::vec3(0.f, 1000.f, 0.f);
    maxHeightDistance = 1.f;
    transferValue = 50.f;

    healthBarHeight = 20.f;
    healthBarWidth = 60.f;

    cameraPosition = glm::vec3(0, 0, 50);
    cameraFocus = 1;

    playerSpeed = 100.f;
    enemySpeed = 50.f;
    playerShootCD = 0.5f;
    enemyShootCD = 1.f;
    playerCannonAngleStep = 60.f;
    enemyCannonAngleStep = 60.f;
    playerMaxHealth = 100;
    enemyMaxHealth = 100;
    enemyAttackRange = 300.f;
    enemyDetectRange = 500.f;
    enemiesNumber = 5;

    fireworkTimer = 0.f;
    fireworkCD = 0.f;
    gameOver = false;
}

Tema1* Tema1::GetInstance() {
    if (instance == nullptr) {
        instance = new Tema1();
    }
    return instance;
}


Tema1::~Tema1()
{
    if (instance) {
        delete instance;
        instance = nullptr;
    }
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
        else {
            name = "enemy";
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

    srand(time(0));

    // Initiate Terrain
    float pointScaleX = (float)resolution.x / maxVisibleX;
    float pointScaleY = (float)resolution.y / maxVisibleY;
    for (float x = 0.f; x <= maxMapX; x += step) {
        heightMap.push_back({x * pointScaleX, TerrainFunc(x) * pointScaleY});
    }

    // Initiate Tanks
    int chunkIndex = (int)(1.f / step);
    float posX = std::get<0>(heightMap[chunkIndex]);
    tank1 = Tank(glm::vec3(posX, 0.f, 0.f), chunkIndex, playerSpeed, playerCannonAngleStep, playerMaxHealth, playerShootCD);

    chunkIndex = (int)(2.f / step);
    posX = std::get<0>(heightMap[chunkIndex]);
    tank2 = Tank(glm::vec3(posX, 0.f, 0.f), chunkIndex, playerSpeed, playerCannonAngleStep, playerMaxHealth, playerShootCD);

    // Initiate Enemies
    for (int i = 1; i <= enemiesNumber; i++) {
        chunkIndex = (int)((maxMapX - excessChunksNumber * step) * i / (enemiesNumber * step));
        posX = std::get<0>(heightMap[chunkIndex]);
        enemies.push_back(Enemy(glm::vec3(posX, 0.f, 0.f), chunkIndex, enemySpeed, enemyCannonAngleStep, enemyMaxHealth, enemyShootCD, enemyAttackRange, enemyDetectRange));
        enemies[i - 1].SetStatus(true);
    }

    // Create Meshes
    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 1;
    
    Mesh* terrain = object2Dmodified::CreateSquare("terrain", corner, squareSide, glm::vec3(0.059, 0.529, 0.075), true);
    Mesh* cannon = object2Dmodified::CreateSquare("cannon", corner, squareSide, glm::vec3(0.11, 0.122, 0.11), true);
    Mesh* tank1 = object2Dmodified::CreateTank("tank1", corner, Tank::width, Tank::height, Tank::radius, glm::vec3(0.31, 0.188, 0.039), glm::vec3(0.871, 0.208, 0.075), true);
    Mesh* tank2 = object2Dmodified::CreateTank("tank2", corner, Tank::width, Tank::height, Tank::radius, glm::vec3(0.31, 0.188, 0.039), glm::vec3(0.071, 0.184, 0.922), true);
    Mesh* enemy = object2Dmodified::CreateTank("enemy", corner, Tank::width, Tank::height, Tank::radius, glm::vec3(0.31, 0.188, 0.039), glm::vec3(0.553, 0.722, 0.184), true);
    Mesh* circle = object2Dmodified::CreateCircle("circle", corner, 1, glm::vec3(0, 0, 0), true);
    Mesh* healthBarBorder = object2Dmodified::CreateSquare("barBorder", corner, squareSide, glm::vec3(1, 1, 1), false);
    Mesh* healthBar = object2Dmodified::CreateSquare("barFill", corner, squareSide, glm::vec3(1, 1, 1), true);
    Mesh* line = object2Dmodified::CreateLine("line", glm::vec3(1, 1, 1));
    Mesh* lineRed = object2Dmodified::CreateLine("lineRed", glm::vec3(1, 0, 0));
    Mesh* lineBlue = object2Dmodified::CreateLine("lineBlue", glm::vec3(0, 0, 1));
    Mesh* lineGreen = object2Dmodified::CreateLine("lineGreen", glm::vec3(0, 1, 0));
    
    AddMeshToList(terrain);
    AddMeshToList(cannon);
    AddMeshToList(tank1);
    AddMeshToList(tank2);
    AddMeshToList(enemy);
    AddMeshToList(circle);
    AddMeshToList(healthBarBorder);
    AddMeshToList(healthBar);
    AddMeshToList(line);
    AddMeshToList(lineRed);
    AddMeshToList(lineBlue);
    AddMeshToList(lineGreen);
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
    int firstChunkIndex;

    tank1.shootTimer += deltaTimeSeconds;
    tank2.shootTimer += deltaTimeSeconds;

    if (tank1.IsDead()) {
        cameraFocus = 2;
    }
    else if (tank2.IsDead()) {
        cameraFocus = 1;
    }

    if (cameraFocus == 1) {
        firstChunkIndex = firstChunkIndex1;
    }
    else {
        firstChunkIndex = firstChunkIndex2;
    }

    // End game fireworks
    if (gameOver) {
        fireworkTimer += deltaTimeSeconds;

        if (fireworkTimer >= fireworkCD) {
            fireworkTimer = 0.f;
            fireworkCD = 0.1f + rand() % 3 / 10.f;

            glm::ivec2 resolution = window->GetResolution();
            int maxX = resolution.x * 3 / 4;
            int minX = resolution.x / 4;
            float posX = std::get<0>(heightMap[firstChunkIndex]) + minX + rand() % (maxX - minX + 1);
            int minY = resolution.y / 2;
            int maxY = resolution.y * 3 / 4;
            float posY = minY + rand() % (maxY - minY + 1);
            float propagationFactor = 1.f + rand() % 40 / 10.f;
            int propagationNumber = 3 + rand() % 3;
            float propagationCD = 0.1f + rand() % 2 / 20.f;
            float scale = 4.f + rand() % 70 / 10.f;
            int raysNumber = 20 + rand() % 30;
            fireworks.push_back(Fireworks(glm::vec3(posX, posY, 0.f), propagationFactor, propagationNumber, propagationCD, scale, raysNumber));
        }

        for (auto it = fireworks.begin(); it != fireworks.end(); ) {
            std::vector<glm::mat3> res = it->Fire(deltaTimeSeconds);
            if (it->IsDead()) {
                it = fireworks.erase(it);
            }
            else {
                for (int i = 0; i < res.size(); i++) {
                    int color = rand() % 3;
                    string name = "line";
                    switch (color) {
                    case 0: {
                        name = "lineRed";
                        break;
                    }
                    case 1: {
                        name = "lineBlue";
                        break;
                    }
                    case 2: {
                        name = "lineGreen";
                        break;
                    }
                    }
                    RenderMesh2D(meshes[name], shaders["VertexColor"], res[i]);
                }
                ++it;
            }
        }
    }

    // Land slide
    for (int i = max(0, firstChunkIndex - excessChunksNumber); i < min(visibleChunksNumber + firstChunkIndex + excessChunksNumber, lastChunkIndex) - 1; i++) {
        ax = std::get<0>(heightMap[i]);
        bx = std::get<0>(heightMap[i + 1]);
        ay = std::get<1>(heightMap[i]);
        by = std::get<1>(heightMap[i + 1]);

        float heightDif = abs(ay - by);

        if (heightDif > maxHeightDistance) {
            float heightTransfer = std::min(transferValue, heightDif * 3.f) * deltaTimeSeconds;

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
    for (int i = 0; i < lastChunkIndex - 1; i++) {
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

    // Updating player tanks
    UpdateTank(tank1, deltaTimeSeconds);
    UpdateTank(tank2, deltaTimeSeconds);

    // Updating enemy tanks
    if (!gameOver) {
        vector<glm::vec3> tanksPositions = { tank1.GetPosition(), tank2.GetPosition() };
        bool allEnemiesDefeated = true;

        for (int i = 0; i < enemiesNumber; i++) {
            float posX = enemies[i].GetPosition().x;
            if (posX >= std::get<0>(heightMap[firstChunkIndex]) &&
                posX <= std::get<0>(heightMap[firstChunkIndex + visibleChunksNumber])) {
                enemies[i].UpdateAI(tanksPositions, heightMap, visibleChunksNumber, firstChunkIndex, lastChunkIndex, excessChunksNumber, gravity, projectiles, projectilesPool, deltaTimeSeconds);
                UpdateTank(enemies[i], deltaTimeSeconds);
            }
            if (!enemies[i].IsDead()) {
                allEnemiesDefeated = false;
            }
        }

        if (allEnemiesDefeated) {
            gameOver = true;
        }
    }

    // Projectiles
    for (auto it = projectiles.begin(); it != projectiles.end(); ) {

        // Projectile rendering
        RenderMesh2D(meshes["circle"], shaders["VertexColor"], it->BuildProjectile(heightMap));

        // Projectile movement
        if (it->Update(heightMap, visibleChunksNumber, firstChunkIndex, lastChunkIndex, excessChunksNumber, tank1, tank2, enemies, projectiles, projectilesPool, gravity, window->GetResolution(), deltaTimeSeconds)) {

            // Remove projectile and add it to the pool
            projectilesPool.push_back(*it);
            it = projectiles.erase(it);
        }
        else {
            ++it;
        }
    }

    // Camera Position
    glm::vec3 targetPos = glm::vec3(std::get<0>(heightMap[firstChunkIndex]), 0, 50);
    glm::vec3 difference = targetPos - cameraPosition;

    // Damping camera movement
    cameraPosition += difference * 0.05f;

    GetSceneCamera()->SetPosition(cameraPosition);
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
        float newPosX = position.x + tank1.GetSpeed() * deltaTime;
        int i = currentChunk + 1;
        float firstChunkPosX = std::get<0>(heightMap[firstChunkIndex1]);
        float lastChunkPosX = std::get<0>(heightMap[lastChunkIndex]);

        if (newPosX <= lastChunkPosX - excessChunksNumber) {
            tank1.SetPosition(glm::vec3(newPosX, position.y, position.z));

            // Update the chunk the tank is on
            for (; i < visibleChunksNumber + firstChunkIndex1; i++) {
                if (std::get<0>(heightMap[i]) > newPosX) {
                    tank1.SetChunkIndex(i - 1);
                    break;
                }
            }

            if (newPosX > resolution.x / 2.f + firstChunkPosX && newPosX < lastChunkPosX - resolution.x / 2.f - excessChunksNumber) {
                int chunkMoveNumber = i - 1 - currentChunk;

                firstChunkIndex1 += chunkMoveNumber;
            }
        }
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        glm::vec3 position = tank1.GetPosition();
        glm::ivec2 resolution = window->GetResolution();
        int currentChunk = tank1.GetChunkIndex();
        float newPosX = position.x - tank1.GetSpeed() * deltaTime;
        int i = currentChunk;
        float firstChunkPosX = std::get<0>(heightMap[firstChunkIndex1]);
        float lastChunkPosX = std::get<0>(heightMap[lastChunkIndex]);

        if (newPosX >= 0) {
            tank1.SetPosition(glm::vec3(newPosX, position.y, position.z));

            // Update the chunk the tank is on
            for (i = currentChunk; i >= firstChunkIndex1; i--) {
                if (std::get<0>(heightMap[i]) <= newPosX) {
                    tank1.SetChunkIndex(i);
                    break;
                }
            }

            if (newPosX < resolution.x / 2.f + firstChunkPosX && newPosX > resolution.x / 2.f) {
                int chunkMoveNumber = i - currentChunk;

                firstChunkIndex1 += chunkMoveNumber;
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
    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        glm::vec3 position = tank2.GetPosition();
        glm::ivec2 resolution = window->GetResolution();
        int currentChunk = tank2.GetChunkIndex();
        float newPosX = position.x + tank2.GetSpeed() * deltaTime;
        int i = currentChunk + 1;
        float firstChunkPosX = std::get<0>(heightMap[firstChunkIndex2]);
        float lastChunkPosX = std::get<0>(heightMap[lastChunkIndex]);

        if (newPosX <= lastChunkPosX - excessChunksNumber) {
            tank2.SetPosition(glm::vec3(newPosX, position.y, position.z));

            // Update the chunk the tank is on
            for (; i < visibleChunksNumber + firstChunkIndex2; i++) {
                if (std::get<0>(heightMap[i]) > newPosX) {
                    tank2.SetChunkIndex(i - 1);
                    break;
                }
            }

            if (newPosX > resolution.x / 2.f + firstChunkPosX && newPosX < lastChunkPosX - resolution.x / 2.f - excessChunksNumber) {
                int chunkMoveNumber = i - 1 - currentChunk;
                firstChunkIndex2 += chunkMoveNumber;
            }
        }
    }
    if (window->KeyHold(GLFW_KEY_LEFT)) {
        glm::vec3 position = tank2.GetPosition();
        glm::ivec2 resolution = window->GetResolution();
        int currentChunk = tank2.GetChunkIndex();
        float newPosX = position.x - tank2.GetSpeed() * deltaTime;
        int i = currentChunk;
        float firstChunkPosX = std::get<0>(heightMap[firstChunkIndex2]);
        float lastChunkPosX = std::get<0>(heightMap[lastChunkIndex]);

        if (newPosX >= 0) {
            tank2.SetPosition(glm::vec3(newPosX, position.y, position.z));

            // Update the chunk the tank is on
            for (; i >= firstChunkIndex2; i--) {
                if (std::get<0>(heightMap[i]) <= newPosX) {
                    tank2.SetChunkIndex(i);
                    break;
                }
            }

            if (newPosX < resolution.x / 2.f + firstChunkPosX && newPosX > resolution.x / 2.f) {
                int chunkMoveNumber = i - currentChunk;

                firstChunkIndex2 += chunkMoveNumber;
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


void Tema1::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_SPACE && !tank1.IsDead()) {
        if (tank1.shootTimer >= tank1.shootCD) {
            tank1.shootTimer = 0.f;
            tank1.FireProjectile(Projectile::projectileSpeed, gravity, projectiles, projectilesPool);
        }
    }
    if (key == GLFW_KEY_ENTER && !tank2.IsDead()) {
        if (tank2.shootTimer >= tank2.shootCD) {
            tank2.shootTimer = 0.f;
            tank2.FireProjectile(Projectile::projectileSpeed, gravity, projectiles, projectilesPool);
        }
    }
    if (key == GLFW_KEY_F) {
        if (cameraFocus == 1) {
            cameraFocus = 2;
        }
        else {
            cameraFocus = 1;
        }
    }
    if (key == GLFW_KEY_P) {
        fireworks.push_back(Fireworks(glm::vec3(200.f, 200.f, 0.f), 6.f, 4.f, 0.1f, 10.f, 30));
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
