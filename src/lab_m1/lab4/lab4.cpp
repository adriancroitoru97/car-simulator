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


    // sun initialize
    tXSun = 0;
    tYSun = 0;
    tZSun = 0;
    aXSun = 0;
    aYSun = 0;
    aZSun = 0;

    aXEarth = 0;
    aYEarth = 0;
    aZEarth = 0;


    // Create a mesh box using custom data
    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(0, 0, 1) - glm::vec3(.5), glm::vec3(.93, .93, 0)),
            // TODO(student): Complete the vertices data for the cube mesh
            VertexFormat(glm::vec3(1, 0, 1) - glm::vec3(.5), glm::vec3(.93, .93, 0)),
            VertexFormat(glm::vec3(0, 1, 1) - glm::vec3(.5), glm::vec3(.93, .93, 0)),
            VertexFormat(glm::vec3(1, 1, 1) - glm::vec3(.5), glm::vec3(.93, .93, 0)),
            VertexFormat(glm::vec3(0, 0, 0) - glm::vec3(.5), glm::vec3(.93, .93, 0)),
            VertexFormat(glm::vec3(1, 0, 0) - glm::vec3(.5), glm::vec3(.93, .93, 0)),
            VertexFormat(glm::vec3(0, 1, 0) - glm::vec3(.5), glm::vec3(.93, .93, 0)),
            VertexFormat(glm::vec3(1, 1, 0) - glm::vec3(.5), glm::vec3(.93, .93, 0)),
        };

        vector<unsigned int> indices =
        {
            0, 1, 2,    // indices for first triangle
            1, 3, 2,    // indices for second triangle
            // TODO(student): Complete indices data for the cube mesh
            2, 3, 7,
            2, 7, 6,
            1, 7, 3,
            1, 5, 7,
            6, 7, 4,
            7, 5, 4,
            0, 4, 1,
            1, 4, 5,
            2, 6, 4,
            0, 2, 4
        };

        meshes["sun"] = new Mesh("generated sun");
        meshes["sun"]->InitFromData(vertices, indices);
    }


    // earth
    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(0, 0, 1) - glm::vec3(.5), glm::vec3(0, .5, 1)),
            // TODO(student): Complete the vertices data for the cube mesh
            VertexFormat(glm::vec3(1, 0, 1) - glm::vec3(.5), glm::vec3(0, .5, 1)),
            VertexFormat(glm::vec3(0, 1, 1) - glm::vec3(.5), glm::vec3(0, .5, 1)),
            VertexFormat(glm::vec3(1, 1, 1) - glm::vec3(.5), glm::vec3(0, .5, 1)),
            VertexFormat(glm::vec3(0, 0, 0) - glm::vec3(.5), glm::vec3(0, .5, 1)),
            VertexFormat(glm::vec3(1, 0, 0) - glm::vec3(.5), glm::vec3(0, .5, 1)),
            VertexFormat(glm::vec3(0, 1, 0) - glm::vec3(.5), glm::vec3(0, .5, 1)),
            VertexFormat(glm::vec3(1, 1, 0) - glm::vec3(.5), glm::vec3(0, .5, 1))
        };

        vector<unsigned int> indices =
        {
            0, 1, 2,    // indices for first triangle
            1, 3, 2,    // indices for second triangle
            // TODO(student): Complete indices data for the cube mesh
            2, 3, 7,
            2, 7, 6,
            1, 7, 3,
            1, 5, 7,
            6, 7, 4,
            7, 5, 4,
            0, 4, 1,
            1, 4, 5,
            2, 6, 4,
            0, 2, 4
        };

        meshes["earth"] = new Mesh("generated earth");
        meshes["earth"]->InitFromData(vertices, indices);
    }

    // moon
    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(0, 0, 1) - glm::vec3(.5), glm::vec3(1, 1, 1)),
            // TODO(student): Complete the vertices data for the cube mesh
            VertexFormat(glm::vec3(1, 0, 1) - glm::vec3(.5), glm::vec3(1, 1, 1)),
            VertexFormat(glm::vec3(0, 1, 1) - glm::vec3(.5), glm::vec3(1, 1, 1)),
            VertexFormat(glm::vec3(1, 1, 1) - glm::vec3(.5), glm::vec3(1, 1, 1)),
            VertexFormat(glm::vec3(0, 0, 0) - glm::vec3(.5), glm::vec3(1, 1, 1)),
            VertexFormat(glm::vec3(1, 0, 0) - glm::vec3(.5), glm::vec3(1, 1, 1)),
            VertexFormat(glm::vec3(0, 1, 0) - glm::vec3(.5), glm::vec3(1, 1, 1)),
            VertexFormat(glm::vec3(1, 1, 0) - glm::vec3(.5), glm::vec3(1, 1, 1))
        };

        vector<unsigned int> indices =
        {
            0, 1, 2,    // indices for first triangle
            1, 3, 2,    // indices for second triangle
            // TODO(student): Complete indices data for the cube mesh
            2, 3, 7,
            2, 7, 6,
            1, 7, 3,
            1, 5, 7,
            6, 7, 4,
            7, 5, 4,
            0, 4, 1,
            1, 4, 5,
            2, 6, 4,
            0, 2, 4
        };

        meshes["moon"] = new Mesh("generated earth");
        meshes["moon"]->InitFromData(vertices, indices);
    }
}

void Lab4::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
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

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    modelMatrix *= transform3D::RotateOY(aYSun);
    RenderMesh(meshes["sun"], shaders["VertexColor"], modelMatrix);

  
    modelMatrix *= transform3D::RotateOY(aYEarth);
    modelMatrix *= transform3D::Translate(4, 0, 0);
    modelMatrix *= transform3D::RotateOY(aYEarth);
    RenderMesh(meshes["earth"], shaders["VertexColor"], modelMatrix);

    modelMatrix *= transform3D::Translate(-2, 0, 0);
    modelMatrix *= transform3D::RotateOY(aZEarth);
    RenderMesh(meshes["moon"], shaders["VertexColor"], modelMatrix);
}

void Lab4::Update(float deltaTimeSeconds)
{
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    aYEarth += deltaTimeSeconds;
    aYSun += deltaTimeSeconds;
    aZEarth += deltaTimeSeconds;

    RenderScene();
    DrawCoordinateSystem();

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);

    // TODO(student): render the scene again, in the new viewport
    DrawCoordinateSystem();
    RenderScene();
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

    // FIRST CUBE
    if (window->KeyHold(GLFW_KEY_W)) {
        translateZ -= deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        translateZ += deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        translateX -= deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        translateX += deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_R)) {
        translateY += deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_F)) {
        translateY -= deltaTime;
    }

    
    // SECOND CUBE
    if (window->KeyHold(GLFW_KEY_1)) {
        scaleX += deltaTime;
        scaleY += deltaTime;
        scaleZ += deltaTime;
    }

    if (window->KeyHold(GLFW_KEY_2)) {
        scaleX -= deltaTime;
        scaleY -= deltaTime;
        scaleZ -= deltaTime;
    }


    // THIRD CUBE
    if (window->KeyHold(GLFW_KEY_3)) {
        angularStepOX += deltaTime * 2;
    }

    if (window->KeyHold(GLFW_KEY_4)) {
        angularStepOX -= deltaTime * 2;
    }

    if (window->KeyHold(GLFW_KEY_5)) {
        angularStepOY += deltaTime * 2;
    }

    if (window->KeyHold(GLFW_KEY_6)) {
        angularStepOY -= deltaTime * 2;
    }

    if (window->KeyHold(GLFW_KEY_7)) {
        angularStepOZ += deltaTime * 2;
    }

    if (window->KeyHold(GLFW_KEY_8)) {
        angularStepOZ -= deltaTime * 2;
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
        //glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);
        miniViewportArea.y += 4;
    }

    if (key == GLFW_KEY_J) {
        miniViewportArea.y -= 4;
    }

    if (key == GLFW_KEY_K) {
        miniViewportArea.x -= 4;
    }

    if (key == GLFW_KEY_L) {
        miniViewportArea.x += 4;
    }

    if (key == GLFW_KEY_U) {
        glm::ivec2 resolution = window->GetResolution();
        miniViewportArea.height += (resolution.y / 5.) / 15;
        miniViewportArea.width += (resolution.x / 5.) / 15;
    }

    if (key == GLFW_KEY_O) {
        glm::ivec2 resolution = window->GetResolution();
        miniViewportArea.height -= (resolution.y / 5.) / 15;
        miniViewportArea.width -= (resolution.x / 5.) / 15;
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
