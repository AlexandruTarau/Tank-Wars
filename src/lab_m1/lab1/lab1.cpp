#include "lab_m1/lab1/lab1.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <numbers>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab1::Lab1()
{
    // TODO(student): Never forget to initialize class variables!
    red = 0;
    green = 0;
    blue = 0;
    cycling_meshes[0] = "sphere";
    cycling_meshes[1] = "box";
    cycling_meshes[2] = "teapot";
    array_index = 0;
    transforming_mesh = cycling_meshes[array_index];
    mesh_position = glm::vec3(0, 0.5f, 0);
    mesh_speed = 0.5f;
    isRotating = false;
    degrees = 20;
    radius = 2;
}


Lab1::~Lab1()
{
}


void Lab1::Init()
{
    // Load a mesh from file into GPU memory. We only need to do it once,
    // no matter how many times we want to draw this mesh.
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // TODO(student): Load some more meshes. The value of RESOURCE_PATH::MODELS
    // is actually a path on disk, go there and you will find more meshes.
    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("teapot");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "teapot.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
}


void Lab1::FrameStart()
{
}


void Lab1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->props.resolution;

    // Sets the clear color for the color buffeasr

    // TODO(student): Generalize the arguments of `glClearColor`.
    // You can, for example, declare three variables in the class header,
    // that will store the color components (red, green, blue).
    glClearColor(red, green, blue, 1);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);

    // Render the object
    RenderMesh(meshes["box"], glm::vec3(1, 0.5f, 0), glm::vec3(0.5f));

    // Render the object again but with different properties
    RenderMesh(meshes["box"], glm::vec3(-1, 0.5f, 0));

    // TODO(student): We need to render (a.k.a. draw) the mesh that
    // was previously loaded. We do this using `RenderMesh`. Check the
    // signature of this function to see the meaning of its parameters.
    // You can draw the same mesh any number of times.
    RenderMesh(meshes[transforming_mesh], mesh_position);

    if (isRotating) {
        float radians = degrees * std::numbers::pi / 180.0f;
        mesh_position.x = radius * cos(radians);
        mesh_position.z = radius * sin(radians);
        degrees += 20 * deltaTimeSeconds;
        if (degrees >= 360) {
            degrees = 0;
        }
    }
}


void Lab1::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab1::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input

    // TODO(student): Add some key hold events that will let you move
    // a mesh instance on all three axes. You will also need to
    // generalize the position used by `RenderMesh`.
    if (window->KeyHold(GLFW_KEY_W)) {
        mesh_position.z -= mesh_speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        mesh_position.z += mesh_speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        mesh_position.x += mesh_speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        mesh_position.x -= mesh_speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_E)) {
        mesh_position.y += mesh_speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_Q)) {
        mesh_position.y -= mesh_speed * deltaTime;
    }
}


void Lab1::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_F) {
        // TODO(student): Change the values of the color components.
        red = 255;
        green = 0;
        blue = 0;
    }

    // TODO(student): Add a key press event that will let you cycle
    // through at least two meshes, rendered at the same position.
    // You will also need to generalize the mesh name used by `RenderMesh`.
    if (key == GLFW_KEY_G) {
        if (++array_index >= sizeof(cycling_meshes) / sizeof(cycling_meshes[0])) {
            array_index = 0;
        }
        transforming_mesh = cycling_meshes[array_index];
    }

    // Rotate mesh on key press
    if (key == GLFW_KEY_R) {
        isRotating = !isRotating;
    }
}


void Lab1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Lab1::OnWindowResize(int width, int height)
{
    // Treat window resize event
}
