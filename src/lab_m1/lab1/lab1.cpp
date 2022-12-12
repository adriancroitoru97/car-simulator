#include "lab_m1/lab1/lab1.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab1::Lab1()
{
    // TODO(student): Never forget to initialize class variables!
    GL_COLOR_RED = 0;
    GL_COLOR_GREEN = 0;
    GL_COLOR_BLUE = 0;
    color_is_changed = false;

    MY_OBJECT = "sphere";
    MY_OBJECT_TYPE = 0;

    MY_MOVING_OBJECT_POS = {5, 5, 1};

    JUMPING_SPEED = 0;
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
        Mesh* mesh2 = new Mesh("teapot");
        mesh2->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "teapot.obj");
        meshes[mesh2->GetMeshID()] = mesh2;
    }

    {
        Mesh* mesh3 = new Mesh("sphere");
        mesh3->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh3->GetMeshID()] = mesh3;
    }
}


void Lab1::FrameStart()
{
}


void Lab1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->props.resolution;

    // Sets the clear color for the color buffer

    // TODO(student): Generalize the arguments of `glClearColor`.
    // You can, for example, declare three variables in the class header,
    // that will store the color components (red, green, blue).
    glClearColor(GL_COLOR_RED, GL_COLOR_GREEN, GL_COLOR_BLUE, 1);

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
    RenderMesh(meshes["teapot"], glm::vec3(-3, -2, 5));

    RenderMesh(meshes[MY_OBJECT], glm::vec3(4, 3, 0));

    RenderMesh(meshes["box"], MY_MOVING_OBJECT_POS);

    if (MY_MOVING_OBJECT_POS.y >= 0) {
        JUMPING_SPEED -= 9.8 * deltaTimeSeconds;
        MY_MOVING_OBJECT_POS.y = MY_MOVING_OBJECT_POS.y + JUMPING_SPEED * deltaTimeSeconds;
    }

    if (MY_MOVING_OBJECT_POS.y < 0) {
        MY_MOVING_OBJECT_POS.y = 0;
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
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        return;
    }

    if (window->KeyHold(GLFW_KEY_W)) {
        MY_MOVING_OBJECT_POS.x = MY_MOVING_OBJECT_POS.x + 1 * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        MY_MOVING_OBJECT_POS.z = MY_MOVING_OBJECT_POS.z - 1 * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        MY_MOVING_OBJECT_POS.x = MY_MOVING_OBJECT_POS.x - 1 * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        MY_MOVING_OBJECT_POS.z = MY_MOVING_OBJECT_POS.z + 1 * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_Q)) {
        MY_MOVING_OBJECT_POS.y = MY_MOVING_OBJECT_POS.y + 1 * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_E)) {
        MY_MOVING_OBJECT_POS.y = MY_MOVING_OBJECT_POS.y - 1 * deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_SPACE)) {
        JUMPING_SPEED = 3;
    }
}


void Lab1::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_F) {
        // TODO(student): Change the values of the color components.
        if (Lab1::color_is_changed) {
            GL_COLOR_RED = 0;
            GL_COLOR_GREEN = 0;
            GL_COLOR_BLUE = 0;
            color_is_changed = false;
        } else {
            GL_COLOR_RED = 0.1;
            GL_COLOR_GREEN = 0.3;
            GL_COLOR_BLUE = 0.14;
            color_is_changed = true;
        } 
    }

    // TODO(student): Add a key press event that will let you cycle
    // through at least two meshes, rendered at the same position.
    // You will also need to generalize the mesh name used by `RenderMesh`.
    if (key == GLFW_KEY_T) {
        if (Lab1::MY_OBJECT_TYPE == 0) {
            MY_OBJECT_TYPE++;
            MY_OBJECT = "teapot";
        } else if(Lab1::MY_OBJECT_TYPE == 1) {
            MY_OBJECT_TYPE++;
            MY_OBJECT = "box";
        } else {
            MY_OBJECT_TYPE = 0;
            MY_OBJECT = "sphere";
        }
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
