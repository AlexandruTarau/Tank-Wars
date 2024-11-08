#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class Lab1 : public gfxc::SimpleScene
    {
     public:
        Lab1();
        ~Lab1();

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

        // TODO(student): Class variables go here
        GLclampf red;
        GLclampf green;
        GLclampf blue;
        glm::vec3 mesh_position;
        float mesh_speed;
        std::string transforming_mesh;
        std::string cycling_meshes[3];
        int array_index;
        bool isRotating;
        float degrees;
        float radius;
    };
}   // namespace m1
