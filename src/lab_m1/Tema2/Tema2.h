#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/transform3D.h"
//#include "lab_m1/Tema2/lab_camera.h"

#include <vector>

namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        struct ViewportArea
        {
            ViewportArea() : x(0), y(0), width(1), height(1) {}
            ViewportArea(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        Tema2();
        ~Tema2();

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

        void RenderScene();
        //void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);

        bool CarIsOnTrack(glm::vec3 car_pos, glm::vec3& closest_segment, glm::vec3& closest_point, float track_width);
        void Tema2::DebugLine(glm::vec3 orig, glm::vec3 tip, glm::vec3 color);

        std::vector<glm::vec3> GenerateObstacleCarPath(int side);
    protected:
        glm::mat4 modelMatrix;
        GLenum polygonMode;
        ViewportArea miniViewportArea;
        bool minimap_state;

        std::vector<glm::vec3> roadPoints;
   
        /* Main Car */
        float car_dir_angle;
        glm::vec3 car_pos, car_dir;
        float car_speed;
        bool car_going_forward;


        /* Camera */
        gfxc::Camera *camera;
        glm::vec3 camera_pos;
        glm::quat camera_dir;
        float FOV;


        /* Obstacle Cars */
        int number_of_obstacles;
        std::vector<glm::vec3> path;
        std::vector<glm::vec3> obstacle_pos, obstacle_dir;
        std::vector<float> obstacle_dir_angle;
        std::vector<float> obstacle_speed;
        std::vector<bool> obstacle_going_forward;


        /* VS uniform location */
        GLint carPosLoc;


        /* Snow Continuously Falling Variables */
        float snow1_tr_down, snow2_tr_down;
    };
}   // namespace m1
