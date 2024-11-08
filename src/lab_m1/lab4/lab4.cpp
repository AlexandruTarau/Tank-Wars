#include "lab_m1/lab4/lab4.h"

#include <vector>
#include <string>
#include <iostream>

#include "lab_m1/lab4/transform3D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab4::Lab4()
{
}


Lab4::~Lab4()
{
}


void Lab4::Init()
{
    polygonMode = GL_FILL;

    Mesh* mesh = new Mesh("box");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
    meshes[mesh->GetMeshID()] = mesh;

    // Initialize tx, ty and tz (the translation steps)
    translateX = 0;
    translateY = 0;
    translateZ = 0;

    // Initialize sx, sy and sz (the scale factors)
    scaleX = 1;
    scaleY = 1;
    scaleZ = 1;

    // Initialize angular steps
    angularStepOX = 0;
    angularStepOY = 0;
    angularStepOZ = 0;

    // Sets the resolution of the small viewport
    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);
}

void Lab4::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Lab4::GrowBranches(int nr, float posX, float posY, float posZ, float scaleX, float scaleY, float scaleZ)
{
    if (nr == 0) {
        return;
    }
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(posX + (1.f / 2.0f) + 2.f / (sqrt(2.f) * 2.f), posY + 2.f + 2.f / (2.f * sqrt(2.f)), posZ);
    modelMatrix *= transform3D::RotateOZ(-3.1415f / 4.f);
    modelMatrix *= transform3D::Scale(scaleX / 2.f, scaleY / 2.f, scaleZ / 2.f);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(posX - (1.f / 2.0f) - 2.f / (sqrt(2.f) * 2.f), posY + 2.f + 2.f / (2.f * sqrt(2.f)), posZ);
    modelMatrix *= transform3D::RotateOZ(3.1415f / 4.f);
    modelMatrix *= transform3D::Scale(scaleX / 2.f, scaleY / 2.f, scaleZ / 2.f);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    //GrowBranches(--nr, posX + (1.f / 2.0f) + 2.f / (sqrt(2.f) * 2.f), posY + 2.f + 2.f / (2.f * sqrt(2.f)), posZ, scaleX / 2.f, scaleY / 2.f, scaleZ / 2.f);
}

void Lab4::RenderScene() {
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(-2.5f, 0.5f, -1.5f);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0.0f, 0.5f, -1.5f);
    modelMatrix *= transform3D::Scale(scaleX, scaleY, scaleZ);
    RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(2.5f, 0.5f, -1.5f);
    modelMatrix *= transform3D::RotateOX(angularStepOX);
    modelMatrix *= transform3D::RotateOY(angularStepOY);
    modelMatrix *= transform3D::RotateOZ(angularStepOZ);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    // Trunchi
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(5.f, 2.f, 5.f);
    modelMatrix *= transform3D::Scale(1, 4, 1);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    GrowBranches(5, 5.f, 2.f, 5.f, 1.f, 4.f, 1.f);
}

void Lab4::Update(float deltaTimeSeconds)
{
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    // Sets the screen area where to draw
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);

    RenderScene();
    DrawCoordinateSystem();

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);

    // TODO(student): render the scene again, in the new viewport
    RenderScene();
    DrawCoordinateSystem();
}

void Lab4::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab4::OnInputUpdate(float deltaTime, int mods)
{
    // TODO(student): Add transformation logic

    // Translate first cube
    if (window->KeyHold(GLFW_KEY_W)) {
        translateZ -= 10 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        translateZ += 10 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        translateX -= 10 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        translateX += 10 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_R)) {
        translateY += 10 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_F)) {
        translateY -= 10 * deltaTime;
    }

    // Scale second cube
    if (window->KeyHold(GLFW_KEY_1)) {
        if (scaleX > 0.5) {
            scaleX -= 5 * deltaTime;
            scaleY = scaleX;
            scaleZ = scaleX;
        }
    }
    if (window->KeyHold(GLFW_KEY_2)) {
        if (scaleX < 5) {
            scaleX += 5 * deltaTime;
            scaleY = scaleX;
            scaleZ = scaleX;
        }
    }

    if (window->KeyHold(GLFW_KEY_3)) {
        if (angularStepOX >= 360) {
            angularStepOX = 0;
        }
        angularStepOX += 5 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_4)) {
      /*  if (angularStepOX <= 0) {
            angularStepOX = 360;
        }*/
        angularStepOX -= 5 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_5)) {
        if (angularStepOY >= 360) {
            angularStepOY = 0;
        }
        angularStepOY += 5 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_6)) {
       /* if (angularStepOY >= 360) {
            angularStepOY = 0;
        }*/
        angularStepOY -= 5 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_7)) {
        if (angularStepOZ >= 360) {
            angularStepOZ = 0;
        }
        angularStepOZ += 5 * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_8)) {
       /* if (angularStepOZ >= 360) {
            angularStepOZ = 0;
        }*/
        angularStepOZ -= 5 * deltaTime;
    }
}


void Lab4::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_SPACE)
    {
        switch (polygonMode)
        {
        case GL_POINT:
            polygonMode = GL_FILL;
            break;
        case GL_LINE:
            polygonMode = GL_POINT;
            break;
        default:
            polygonMode = GL_LINE;
            break;
        }
    }
    
    // TODO(student): Add viewport movement and scaling logic
    if (key == GLFW_KEY_I) {
        miniViewportArea.y += 5;
    }
    if (key == GLFW_KEY_J) {
        miniViewportArea.y -= 5;
    }
    if (key == GLFW_KEY_K) {
        miniViewportArea.x += 5;
    }
    if (key == GLFW_KEY_L) {
        miniViewportArea.x -= 5;
    }
    if (key == GLFW_KEY_U) {
        miniViewportArea.height -= 5;
        miniViewportArea.width -= 5;
    }
    if (key == GLFW_KEY_O) {
        miniViewportArea.height += 5;
        miniViewportArea.width += 5;
    }
}


void Lab4::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab4::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab4::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab4::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab4::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab4::OnWindowResize(int width, int height)
{
}
