#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema1/Projectile.h"
#include "lab_m1/Tema1/Tank.h"
#include "lab_m1/Tema1/Enemy.h"
#include <vector>
#include <tuple>

namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
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
        void UpdateTank(Tank& tank, float deltaTime);
        glm::vec3 ProjectileFunc(glm::vec3 velocity, glm::vec3 gravity, glm::vec3 position, float t);

    protected:
        glm::mat3 modelMatrix;

        // My vars:
        std::vector<std::tuple<float, float>> heightMap;
        float xMax, yMax, step;

        std::vector<Projectile> projectiles;
        std::vector<Projectile> projectilesPool;
        
        glm::vec3 gravity;
        float maxHeightDistance;
        float transferValue;

        Tank tank1, tank2;

        float healthBarHeight, healthBarWidth;
    };
}   // namespace m1
