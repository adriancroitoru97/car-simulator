#pragma once

#include "components/simple_scene.h"
#include "components/text_renderer.h"


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

        /* Meshes Initialization */
        void Init_Meshes();

        /* Translate Matrices */
        void BirdMatrix();
        void WingMatrix();
        void CollisionMatrix();
        void CrosshairMatrix();

        /* Gameplay Scenarios */
        void BirdEscape(float deltaTimeSeconds);
        void BirdShot(float deltaTimeSeconds);

        /* State Reset Functions */
        void BirdResetState();
        void GameResetState();

        /* Bird Physics */
        void WingMotion(float deltaTimeSeconds);
        void BirdReflection();
        void SlowMotionFeature(float& deltaTimeSeconds);

        /* Render Functions */
        void RenderScene();
        void HeartsRender();
        void BlackHeartsRender();
        void BulletsRender();
        void ScoreRender();
        void SlowMotionBarRender();
        void GrassRender();
        void GameOver();
    protected:
        /* Bird Graphics */
        glm::mat3 birdMatrix;
        glm::vec2 birdPos;
        glm::vec2 birdSpeed;
        float speedMagnitude;
        float birdAngle;

        /* Bird Wings Graphics */
        glm::mat3 wingMatrix;
        float cWingX, cWingY;
        float wingScale;
        bool wingState;

        /* Collision */
        glm::mat3 collisionMatrix;
        
        /* HUD */
        glm::mat3 crosshairMatrix, heartMatrix, bulletMatrix,
            scoreBarMatrix, scoreBarSegmentMatrix,
            slowMotionBarMatrix, slowMotionBarSegmentMatrix,
            grassMatrix, blackHeartMatrix;
        float crosshairX, crosshairY;
        int nrHearts, nrBullets;

        gfxc::TextRenderer *tr;
        float initResX, initResY;

        /* Gameplay Variables */
        int score;
        int level;
        bool gameOver;

        float resetSceneTimer;
        float birdTime;
        bool birdIsDead;
        bool birdIsResettingState;

        /* Gameplay Bonuses Variables */
        bool level5Bonus, level6Bonus;
        bool level7Bonus;
        int lvl7_BirdLives;

        bool level8Bonus;
        bool slowMotionActivated;
        float slowMotionFactor;
        float slowMotionTime;
    };
}   // namespace m1
