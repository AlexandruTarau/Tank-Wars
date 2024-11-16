#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema1/Projectile.h"
#include "lab_m1/Tema1/Tank.h"
#include "lab_m1/Tema1/Enemy.h"
#include "lab_m1/Tema1/Fireworks.h"
#include <vector>
#include <tuple>

namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        static Tema1* GetInstance();
        
        ~Tema1();

        void Init() override;

        void UpdateTank(Tank& tank, float deltaTime);

    private:
        static Tema1* instance;

        Tema1();

        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        float TerrainFunc(float x);
        glm::vec3 ProjectileFunc(glm::vec3 velocity, glm::vec3 gravity, glm::vec3 position, float t);

    protected:
        glm::mat3 modelMatrix;

        // My vars:
        std::vector<std::tuple<float, float>> heightMap;
        float maxVisibleX, maxVisibleY, step;
        float maxMapX;
        int visibleChunksNumber;
        int firstChunkIndex1, firstChunkIndex2;
        int lastChunkIndex;
        int excessChunksNumber;

        std::vector<Projectile> projectiles;
        std::vector<Projectile> projectilesPool;

        std::vector<Fireworks> fireworks;
        float fireworkCD;
        float fireworkTimer;
        
        glm::vec3 gravity;
        float maxHeightDistance;
        float transferValue;

        Tank tank1;
        Tank tank2;
        std::vector<Enemy> enemies;
        int enemiesNumber;
        float playerSpeed, enemySpeed;
        float playerShootCD, enemyShootCD;
        float playerCannonAngleStep, enemyCannonAngleStep;
        int playerMaxHealth, enemyMaxHealth;
        float enemyDetectRange, enemyAttackRange;

        float healthBarHeight, healthBarWidth;

        glm::vec3 cameraPosition;
        int cameraFocus;  // 1 = tank1 ; 2 = tank2

        bool gameOver;
    };
}   // namespace m1
