#include "lab_m1/Tema1/Tema1.h"
#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/meshes.h"

#include <vector>
#include <iostream>
#include <time.h>
#include<windows.h>

using namespace std;
using namespace m1;
using namespace gfxc;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}

void Tema1::Init()
{
    /* Camera initialization */
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    /* Meshes initialization */ 
    Init_Meshes();

    /* Wings movement initial settings */ 
    cWingX = -54.527;
    cWingY = 67.4184;
    wingScale = 1;
    wingState = 1;

    /* Bird initial speed */
    speedMagnitude = 300;

    /* Reset bird and game states */
    GameResetState();
    BirdResetState();

    /* Initial gameplay settings */
    score = 0;
    level = 1;
    nrBullets = 3;
    nrHearts = 3;
    gameOver = false;

    /* Bonuses Variables */
    level5Bonus = false;
    level6Bonus = false;
    level7Bonus = false;
    level8Bonus = false;
    slowMotionActivated = false;
    lvl7_BirdLives = 3;
    slowMotionFactor = 1;
    slowMotionTime = 0;

    /* Text Renderer initialization*/
    initResX = resolution.x;
    initResY = resolution.y;
    tr = new TextRenderer(window->props.selfDir,
                                                   window->GetResolution().x,
                                                   window->GetResolution().y);
    tr->Load(window->props.selfDir + "\\assets\\fonts\\Hack-Bold.ttf", 128);
}

void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(238.f/255, 228.f/255, 228.f/255, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    GetSceneCamera()->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::Init_Meshes() {
    {
        pair<vector<VertexFormat>, vector<unsigned int>> mesh = meshes::Bird();
        meshes["bird_body"] = new Mesh("generated bird body");
        meshes["bird_body"]->InitFromData(mesh.first, mesh.second);
    }
    {
        pair<vector<VertexFormat>, vector<unsigned int>> mesh = meshes::KingBird();
        meshes["king_bird_body"] = new Mesh("generated king bird body");
        meshes["king_bird_body"]->InitFromData(mesh.first, mesh.second);
    }
    {
        pair<vector<VertexFormat>, vector<unsigned int>> mesh = meshes::Wing1();
        meshes["bird_wing1"] = new Mesh("generated bird wing1");
        meshes["bird_wing1"]->InitFromData(mesh.first, mesh.second);
    }
    {
        pair<vector<VertexFormat>, vector<unsigned int>> mesh = meshes::Wing2();
        meshes["bird_wing2"] = new Mesh("generated bird wing2");
        meshes["bird_wing2"]->InitFromData(mesh.first, mesh.second);
    }
    {
        pair<vector<VertexFormat>, vector<unsigned int>> mesh = meshes::CollisionBox();
        meshes["collision_box"] = new Mesh("generated collision box");
        meshes["collision_box"]->SetDrawMode(GL_LINES);
        meshes["collision_box"]->InitFromData(mesh.first, mesh.second);
    }
    {
        pair<vector<VertexFormat>, vector<unsigned int>> mesh = meshes::Crosshair();
        meshes["crosshair"] = new Mesh("generated crosshair");
        meshes["crosshair"]->InitFromData(mesh.first, mesh.second);
    }
    {
        pair<vector<VertexFormat>, vector<unsigned int>> mesh = meshes::Heart();
        meshes["heart"] = new Mesh("generated heart");
        meshes["heart"]->InitFromData(mesh.first, mesh.second);
    }
    {
        pair<vector<VertexFormat>, vector<unsigned int>> mesh = meshes::BlackHeart();
        meshes["black_heart"] = new Mesh("generated black heart");
        meshes["black_heart"]->InitFromData(mesh.first, mesh.second);
    }
    {
        pair<vector<VertexFormat>, vector<unsigned int>> mesh = meshes::Bullet();
        meshes["bullet"] = new Mesh("generated bullet");
        meshes["bullet"]->InitFromData(mesh.first, mesh.second);
    }
    {
        pair<vector<VertexFormat>, vector<unsigned int>> mesh = meshes::ScoreBar();
        meshes["score_bar"] = new Mesh("generated score bar");
        meshes["score_bar"]->InitFromData(mesh.first, mesh.second);
    }
    {
        pair<vector<VertexFormat>, vector<unsigned int>> mesh = meshes::ScoreBarSegment();
        meshes["score_bar_segment"] = new Mesh("generated score bar segment");
        meshes["score_bar_segment"]->InitFromData(mesh.first, mesh.second);
    }
    {
        pair<vector<VertexFormat>, vector<unsigned int>> mesh = meshes::SlowMotionSegment();
        meshes["slow_motion_segment"] = new Mesh("generated slow motion bar segment");
        meshes["slow_motion_segment"]->InitFromData(mesh.first, mesh.second);
    }
    {
        pair<vector<VertexFormat>, vector<unsigned int>> mesh = meshes::GrassBlade();
        meshes["grass_blade"] = new Mesh("generated grass blade");
        meshes["grass_blade"]->InitFromData(mesh.first, mesh.second);
    }
}

void Tema1::BirdMatrix() {
    birdMatrix = glm::mat3(1);
    birdMatrix *= transform2D::Translate(birdPos.x, birdPos.y);
    birdMatrix *= transform2D::Scale(.4, .4);
    birdMatrix *= transform2D::Rotate(atan2(birdSpeed.y, birdSpeed.x));
    if (birdSpeed.x < 0) {
        birdMatrix *= transform2D::Scale(1, -1);
    }
    birdMatrix *= transform2D::Rotate(-57.5187f);
}

void Tema1::WingMatrix() {
    wingMatrix = birdMatrix;
    wingMatrix *= transform2D::Translate(cWingX, cWingY);
    wingMatrix *= transform2D::Rotate(1.04f);
    wingMatrix *= transform2D::Scale(1, wingScale);
    wingMatrix *= transform2D::Rotate(-1.04f);
    wingMatrix *= transform2D::Translate(-cWingX, -cWingY);
}

void Tema1::CollisionMatrix() {
    collisionMatrix = birdMatrix;
    collisionMatrix *= transform2D::Translate(-160.796f, 300.035f);
    collisionMatrix *= transform2D::Scale(1.025f, 1.423f);
}

void Tema1::CrosshairMatrix() {
    crosshairMatrix = glm::mat3(1);
    crosshairMatrix *= transform2D::Translate(crosshairX, crosshairY);
    crosshairMatrix *= transform2D::Scale(2.8f, 2.8f);
}

void Tema1::WingMotion(float deltaTimeSeconds) {
    if (wingState) {
        wingScale -= (deltaTimeSeconds * (float)speedMagnitude / 50);
    }
    else {
        wingScale += (deltaTimeSeconds * (float)speedMagnitude / 50);
    }
    if (wingScale <= -0.6f) {
        wingState = 0;
        wingScale = -0.6f;
    }
    if (wingScale >= 1.1f) {
        wingState = 1;
        wingScale = 1.1f;
    }
}

void Tema1::BirdReflection() {
    glm::ivec2 resolution = window->GetResolution();

    if (birdPos.x >= 0.93 * resolution.x) {
        birdSpeed.x = -abs(birdSpeed.x);
        birdSpeed.y += (rand() % 100 - 50);
    }
    if (birdPos.x <= 0.05 * resolution.x) {
        birdSpeed.x = abs(birdSpeed.x);
        birdSpeed.y += (rand() % 100 - 50);
    }
    if (birdPos.y >= 0.90 * resolution.y && birdTime > 0) {
        birdSpeed.y = -abs(birdSpeed.y);
        birdSpeed.x += (rand() % 100 - 50);
    }
    if (birdPos.y <= 0.07 * resolution.y) {
        birdSpeed.y = abs(birdSpeed.y);
        birdSpeed.x += (rand() % 100 - 50);
    }
    birdSpeed = glm::normalize(birdSpeed) * (float)speedMagnitude;
}

void Tema1::HeartsRender() {
    glm::ivec2 resolution = window->GetResolution();

    for (int i = 0; i < nrHearts; i++) {
        heartMatrix = glm::mat3(1);
        heartMatrix *= transform2D::Translate((i + 1) * resolution.x / 24, resolution.y / 1.08f);
        heartMatrix *= transform2D::Scale(0.18f, 0.18f);
        RenderMesh2D(meshes["heart"], shaders["VertexColor"], heartMatrix);
    }
}

void Tema1::BlackHeartsRender() {
    glm::ivec2 resolution = window->GetResolution();

    for (int i = 0; i < lvl7_BirdLives; i++) {
        blackHeartMatrix = glm::mat3(1);
        blackHeartMatrix *= transform2D::Translate(resolution.x * 0.015f + (i + 1) * resolution.x / 32, resolution.y / 1.2f);
        blackHeartMatrix *= transform2D::Scale(0.1f, 0.1f);
        RenderMesh2D(meshes["black_heart"], shaders["VertexColor"], blackHeartMatrix);
    }
}

void Tema1::BulletsRender() {
    glm::ivec2 resolution = window->GetResolution();

    for (int i = 0; i < nrBullets; i++) {
        bulletMatrix = glm::mat3(1);
        bulletMatrix *= transform2D::Translate((i + 0.77f) * resolution.x / 24, resolution.y / 14);
        bulletMatrix *= transform2D::Scale(0.22f, 0.22f);
        RenderMesh2D(meshes["bullet"], shaders["VertexColor"], bulletMatrix);
    }
}

void Tema1::ScoreRender() {
    glm::ivec2 resolution = window->GetResolution();

    scoreBarMatrix = glm::mat3(1);
    scoreBarMatrix *= transform2D::Translate(resolution.x * 0.81f, resolution.y * 0.965f);
    scoreBarMatrix *= transform2D::Scale(1.4f, 1.6f);
    RenderMesh2D(meshes["score_bar"], shaders["VertexColor"], scoreBarMatrix);

    scoreBarSegmentMatrix = scoreBarMatrix;
    scoreBarSegmentMatrix *= transform2D::Translate(3, -38);
    scoreBarSegmentMatrix *= transform2D::Scale(score, 1);
    RenderMesh2D(meshes["score_bar_segment"], shaders["VertexColor"], scoreBarSegmentMatrix);

    /* Level render */
    float scaleResX = initResX / resolution.x;
    float scaleResY = initResY / resolution.y;
    if (scaleResX != 1) scaleResX -= 0.02f;
    if (scaleResY != 1) scaleResY -= 0.1f;
    tr->RenderText("LEVEL: " + to_string(level),
        resolution.x * 0.83f * scaleResX,
        resolution.y * 0.14f * scaleResY,
        .25f);
}

void Tema1::SlowMotionBarRender() {
    glm::ivec2 resolution = window->GetResolution();

    slowMotionBarMatrix = glm::mat3(1);
    slowMotionBarMatrix *= transform2D::Translate(resolution.x * 0.81f, resolution.y * 0.11f);
    slowMotionBarMatrix *= transform2D::Scale(1.4f, 1.6f);
    RenderMesh2D(meshes["score_bar"], shaders["VertexColor"], slowMotionBarMatrix);

    slowMotionBarSegmentMatrix = slowMotionBarMatrix;
    slowMotionBarSegmentMatrix *= transform2D::Translate(3, -38);
    slowMotionBarSegmentMatrix *= transform2D::Scale(6.f * (slowMotionTime) / 5, 1);
    RenderMesh2D(meshes["slow_motion_segment"], shaders["VertexColor"], slowMotionBarSegmentMatrix);
}

void Tema1::GrassRender() {
    float posX = 0;
    glm::ivec2 resolution = window->GetResolution();

    
    while (posX <= resolution.x + 5) {
        grassMatrix = glm::mat3(1);
        grassMatrix *= transform2D::Translate(posX, 0);
        grassMatrix *= transform2D::Scale(0.1f, 1 /*(float)rand() / RAND_MAX */);
        RenderMesh2D(meshes["grass_blade"], shaders["VertexColor"], grassMatrix);

        posX += (resolution.x / 700.f);
    }
}

void Tema1::GameResetState() {
    /* Check if the game is over */
    /*if (nrHearts <= 0) {
        gameOver = true;
    }*/

    /* Update level */
    if (score > 5 || (level == 7 && birdIsDead)) {
        level++;
        level8Bonus = false;
        speedMagnitude *= 1.3f;
        score = 0;
    }

    /* Bonus Level 5 */
    if (level == 5 && !level5Bonus) {
        nrHearts++;
        level5Bonus = true;
    }

    /* Bonus Level 6 */
    if (level == 6 && !level6Bonus) {
        nrHearts++;
        level6Bonus = true;
    }

    /* Bonus Level 7 */
    if (level == 7 && !birdIsDead) {
        gameOver = true;
    }

    /* Bullets Update */
    if (level < 6) {
        nrBullets = 3;
    }
    else {
        nrBullets = 4;
    }

    resetSceneTimer = 0;
}

void Tema1::BirdResetState() {
    glm::ivec2 resolution = window->GetResolution();

    /* Bird initial position */
    birdPos = { 0.05f * resolution.x, 0.05f * resolution.y };

    birdTime = 7;
    birdIsDead = false;
    birdIsResettingState = false;
    
    /* Random bird beginning angle */
    srand(time(NULL));
    for (int i = 0; i < rand() % 100; i++) {
        birdAngle = ((float)rand() / RAND_MAX) * AI_MATH_TWO_PI;
    }
    while (sin(birdAngle) <= 0.3) {
        birdAngle = ((float)rand() / RAND_MAX) * AI_MATH_TWO_PI;
    }

    /* Random bird starting point */
    while (birdPos.x < 0.2 * resolution.x || birdPos.x > 0.8 * resolution.x) {
        birdPos.x = rand() % resolution.x;
    }

    birdSpeed.x = cos(birdAngle) * speedMagnitude;
    birdSpeed.y = sin(birdAngle) * speedMagnitude;
}

void Tema1::GameOver() {
    glm::ivec2 resolution = window->GetResolution();

    float scaleResX = initResX / resolution.x;
    float scaleResY = initResY / resolution.y;
    tr->RenderText("GAME OVER",
        resolution.x / 2 * scaleResX * 0.45f,
        resolution.y / 2 * scaleResY * 0.6f,
        1);
    tr->RenderText("PRESS `R` TO START AGAIN",
        resolution.x / 2 * scaleResX * 0.58f,
        resolution.y / 2 * scaleResY * 0.95f,
        0.3f);
}

void Tema1::RenderScene() {
    /* Render the proper bird according to the level */
    if (level == 7) {
        RenderMesh2D(meshes["king_bird_body"], shaders["VertexColor"], birdMatrix);
        BlackHeartsRender();
    }
    else {
        RenderMesh2D(meshes["bird_body"], shaders["VertexColor"], birdMatrix);
    }

    if (level >= 8) {
        SlowMotionBarRender();
    }

    RenderMesh2D(meshes["bird_wing1"], shaders["VertexColor"], wingMatrix);
    RenderMesh2D(meshes["bird_wing2"], shaders["VertexColor"], wingMatrix);
    RenderMesh2D(meshes["crosshair"], shaders["VertexColor"], crosshairMatrix);

    HeartsRender();
    BulletsRender();
    ScoreRender();
    GrassRender();
}

void Tema1::BirdEscape(float deltaTimeSeconds) {
    glm::ivec2 resolution = window->GetResolution();

    /* Under 2 seconds the bird starts going up */
    if (birdTime <= 2 || nrBullets <= 0) {
        birdSpeed = glm::normalize(birdSpeed) * speedMagnitude;
        birdSpeed.x = 0;
        birdSpeed.y = abs(birdSpeed.y);
    }

    /* Bird escaped */
    if ((birdPos.y > resolution.y * 1.25f || nrBullets <= 0) && !birdIsDead) {
        if (!birdIsResettingState) {
            nrHearts--;
            birdIsResettingState = true;
        }

        resetSceneTimer += deltaTimeSeconds;
        if (resetSceneTimer >= 2) {
            GameResetState();
            BirdResetState();
        }
    }
}

void Tema1::BirdShot(float deltaTimeSeconds) {
    if (birdIsDead) {
        birdSpeed = glm::normalize(birdSpeed) * speedMagnitude;
        birdSpeed.x = 0;
        birdSpeed.y = -abs(birdSpeed.y);

        resetSceneTimer += deltaTimeSeconds;
        if (resetSceneTimer >= 3) {
            GameResetState();
            BirdResetState();
        }
    }
}

void Tema1::SlowMotionFeature(float &deltaTimeSeconds) {
    /* SM maximum 5 */

    if (level == 8 && !level8Bonus) {
        slowMotionTime = 5;
        level8Bonus = true;
    }
    else if (level > 8 && !level8Bonus) {
        if (slowMotionTime <= 0) {
            slowMotionTime = 2;
        }
        else if (slowMotionTime + 2 >= 5) {
            slowMotionTime = 5;
        }
        else {
            slowMotionTime += 2;
        }
        level8Bonus = true;
    }

    if (slowMotionTime > 0 && slowMotionActivated) {
        slowMotionTime -= deltaTimeSeconds;
        deltaTimeSeconds *= slowMotionFactor;
    }

    if (slowMotionTime <= 0) {
        slowMotionActivated = false;
    }
}

void Tema1::Update(float deltaTimeSeconds)
{
    /* SLOW MOTION Feature - level 8+ */
    SlowMotionFeature(deltaTimeSeconds);

    /* Compute transformation matrices */ 
    BirdMatrix();
    WingMatrix();
    CollisionMatrix();
    CrosshairMatrix();

    /* Compute motion and physics */
    WingMotion(deltaTimeSeconds);
    BirdReflection();

    /* Render the proper scene */
    if (nrHearts <= 0) {
        gameOver = true;
    }
    if (gameOver) {
        GameOver();
    }
    else {
        RenderScene();
    }

    /* Bird escapes scenario */
    BirdEscape(deltaTimeSeconds);

    /* Bird is shot scenario */
    BirdShot(deltaTimeSeconds);

    /* Update the bird position and the remaining on-screen time */
    birdPos += birdSpeed * deltaTimeSeconds;
    birdTime -= deltaTimeSeconds;
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
}


void Tema1::OnKeyPress(int key, int mods)
{
    /* Reset button when it is game over */
    if (key == GLFW_KEY_R && gameOver) {
        Init();
    }

    /* SLOW MOTION Button for level 8+ */
    /* Third condition for not allowing SM in tranzitions */
    if (key == GLFW_KEY_LEFT_SHIFT && level >= 8 && resetSceneTimer == 0) {
        if (slowMotionActivated) {
            slowMotionFactor = 1;
            slowMotionActivated = false;
        }
        else {
            slowMotionFactor = 0.25f;
            slowMotionActivated = true;
        }
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    /* Allign crosshair with mouse position */
    glm::ivec2 resolution = window->GetResolution();
    crosshairX = mouseX;
    crosshairY = resolution.y - mouseY;
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    /* Shooting feature */
    if (nrBullets > 0 && button == GLFW_MOUSE_BUTTON_2) {
        glm::ivec2 resolution = window->GetResolution();

        glm::vec2 point1 = collisionMatrix * glm::vec3{ 0, -300, 1 };
        glm::vec2 point2 = collisionMatrix * glm::vec3{ 0, 0, 1 };
        glm::vec2 point3 = collisionMatrix * glm::vec3{ 300, -300, 1 };
        glm::vec2 mouse = {(float)mouseX - point1.x, resolution.y - (float)mouseY - point1.y};

        float project_A = glm::dot(mouse, glm::normalize(point3 - point1));
        float project_B = glm::dot(mouse, glm::normalize(point2 - point1));

        if (project_A > 0 && project_A < glm::length(point3 - point1) &&
            project_B > 0 && project_B < glm::length(point2 - point1)) {
            if (!birdIsDead) {
                score++;
            }

            /* Bird dies if shot usually, excepting lvl7, where it has 3 lives */
            /* When bird dies, SM automatically dezactivated */
            if (level != 7) {
                birdIsDead = true;
                if (slowMotionActivated) {
                    slowMotionFactor = 1;
                    slowMotionActivated = false;
                }
            } else {
                lvl7_BirdLives--;
                if (lvl7_BirdLives == 0) {
                    birdIsDead = true;
                }
            }
        }

        nrBullets--;
    }
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
