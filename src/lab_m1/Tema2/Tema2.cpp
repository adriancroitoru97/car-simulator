#include "lab_m1/Tema2/Tema2.h"
#include "lab_m1/Tema2/meshes.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


bool Tema2::CarIsOnTrack(glm::vec3 car_pos, glm::vec3 &closest_segment, glm::vec3& closest_point, float track_width) {

    Mesh* road_axis = meshes["road_line"];
    vector<VertexFormat> road_axis_vertices = road_axis->vertices;

    float len_min = 1000000;
    for (int i = 0; i < road_axis_vertices.size() - 1; i++) {
        glm::vec3 p1 = road_axis_vertices[i].position;
        glm::vec3 p2 = road_axis_vertices[i + 1].position;

        float u = (car_pos.x - p1.x) * (p2.x - p1.x) + (car_pos.z - p1.z) * (p2.z - p1.z);
        float l = glm::length(p2 - p1);
        u = u / (l * l);

        float len;
        if (u < 0) {
            len = glm::length(p1 - car_pos);
        }
        else if (u > 1) {
            len = glm::length(p2 - car_pos);
        }
        else {
            float x = p1.x + u * (p2.x - p1.x);
            float y = p1.z + u * (p2.z - p1.z);
            len = glm::length(glm::vec3{ x, 0, y } - car_pos);
        }

        if (len < len_min) {
            len_min = len;
            closest_segment = p2 - p1;
            closest_point = p1;
        }
   
        if (len < (track_width / 2)) {
            return true;
        }
    }

    return false;
}


/* side == 1 for same side, -1 for opposite*/
vector<glm::vec3> Tema2::GenerateObstacleCarPath(int side) {
    vector<VertexFormat> road_axis = meshes["road_line"]->vertices;

    std::vector<glm::vec3> points;
    for (int i = 0; i < road_axis.size() - 1; i++) {
        glm::vec3 P1 = road_axis[i].position;
        glm::vec3 P2 = road_axis[i + 1].position;
        glm::vec3 D = glm::normalize(P2 - P1);
        glm::vec3 P = glm::cross(D, glm::vec3(0, 1, 0));

        glm::vec3 exterior = P1 + (float)side * P;

        points.push_back(exterior);
    }

    return points;
}


void Tema2::Init()
{
    /* General Settings Init */
    glm::ivec2 resolution = window->GetResolution();
    polygonMode = GL_FILL;
    miniViewportArea = ViewportArea(30, 30, resolution.x / 7.f, resolution.y / 3.f);
    minimap_state = false;

    /* Custom Shader Load */
    {
        Shader* shader = new Shader("MyShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::SHADERS, "My.Shader.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::SHADERS, "VertexColor.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
        carPosLoc = shaders["MyShader"]->GetUniformLocation("CarPos");
    }
    
    /* Main Car Physics Initialization */
    {
        car_pos = glm::vec3(0);
        car_dir_angle = 3 * AI_MATH_HALF_PI;
        car_dir = glm::vec3(cos(car_dir_angle), 0, sin(car_dir_angle));
        car_speed = 0;
        car_going_forward = true;
    }

    /* Third Person Camera Settings Initialization */
    {
        FOV = 85;
        camera = GetSceneCamera();
        camera->SetPerspective(FOV, window->props.aspectRatio, 0.1f, 200.f);
        GetCameraInput()->SetActive(false);
        camera_pos = car_pos - car_dir + glm::vec3(0, 1, 0);
        camera_dir = glm::quatLookAt(glm::normalize(car_pos + car_dir - camera_pos), glm::vec3(0, 1, 0));
        camera->SetPositionAndRotation(camera_pos, camera_dir);
        camera->Update();
    }
   
    /* Road Mesh Initialization */
    {
        pair<Mesh*, Mesh*> mesh_road = meshes::CreateRoad(2.f, glm::vec3(0.31, 0.31, 0.31), glm::vec3(0.38, 0.38, 0.38));
        meshes["road"] = mesh_road.first;
        meshes["road_line"] = mesh_road.second;
    }

    /* Car Mesh Initialization */
    {
        meshes["car"] = meshes::CreateCar();
    }

    /* Terrain Mesh Initialization */
    {
        meshes["terrain"] = meshes::CreateTerrain(500, std::vector<glm::vec3>{ glm::vec3(1, 0.9804, 0.9804), glm::vec3(236.f / 255, 232.f / 255, 232.f / 255), glm::vec3(233.f / 255, 233.f / 255, 233.f / 255) });
    }

    /* Forest Mesh Initialization */
    {
        meshes["forest"] = meshes::CreateForest(300, 4, meshes["road_line"]);
    }

    /* Fence Mesh Initialization */
    {
        meshes["fence"] = meshes::CreateFence();
    }

    /* Snow Mesh Initialization*/
    {
        //snow_translate_down = 0;
        snow1_tr_down = 0;
        snow2_tr_down = 98;
        meshes["snow"] = meshes::CreateSnow();
    }


    /* Obstacle Cars Settings Init */
    {
        number_of_obstacles = 5;
        path = GenerateObstacleCarPath(-1);
        for (int i = 0; i < number_of_obstacles / 2 + 1; i++) {

            int val = 0;
            while (val < 10 || val > 1000) {
                val = path.size() * static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
            }

            glm::vec3 p1 = path[val];
            glm::vec3 p2 = path[val - 1];
            obstacle_pos.push_back(path[val]);
            float temp_cos = glm::dot(p1 - p2, glm::vec3(0, 0, 1)) / (glm::length(p1 - p2));
            obstacle_dir_angle.push_back(acos(temp_cos));
            obstacle_dir.push_back(p1 - p2);
            obstacle_speed.push_back(0);

            obstacle_going_forward.push_back(true);
        }

        path = GenerateObstacleCarPath(1);
        for (int i = number_of_obstacles / 2 + 1; i < number_of_obstacles; i++) {

            int val = 0;
            while (val < 10 || val > 1000) {
                val = path.size() * static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
            }

            glm::vec3 p1 = path[val];
            glm::vec3 p2 = path[val - 1];
            obstacle_pos.push_back(path[val]);
            float temp_cos = glm::dot(p2 - p1, glm::vec3(0, 0, 1)) / (glm::length(p2 - p1));
            obstacle_dir_angle.push_back(acos(temp_cos));
            obstacle_dir.push_back(p2 - p1);
            obstacle_speed.push_back(0);

            obstacle_going_forward.push_back(true);
        }
    }
}

void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    //glClearColor(113.f/255, 142.f/255, 187.f/255, 1);
    //glClearColor(22.f / 255, 28.f / 255, 37.f / 255, 1);
    glClearColor(11.f / 255, 14.f / 255, 18.f / 255, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::RenderScene() {
    /* Render terrain*/
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(200, 0, 200);
    modelMatrix *= transform3D::RotateOY(AI_MATH_PI);
    RenderMesh(meshes["terrain"], shaders["MyShader"], modelMatrix);

    /* Render forest */
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0, 1.8f, 0);
    modelMatrix *= transform3D::Scale(1, 1.2f, 1);
    RenderMesh(meshes["forest"], shaders["MyShader"], modelMatrix);

    /* Render road and road axis */
    glLineWidth(1.2f);
    RenderMesh(meshes["road"], shaders["MyShader"], transform3D::Translate(0, 0.05f, 0));
    RenderMesh(meshes["road_line"], shaders["MyShader"], transform3D::Translate(0, 0.05f, 0));
    glLineWidth(3);


    /* Render the main car, considering the changes in position and direction */
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(car_pos.x, car_pos.y + 0.101f, car_pos.z);
    modelMatrix *= transform3D::RotateOY(-car_dir_angle);
    modelMatrix *= transform3D::Scale(0.3f, 0.3f, 0.3f);
    modelMatrix *= transform3D::RotateOY(AI_MATH_HALF_PI);
    RenderMesh(meshes["car"], shaders["MyShader"], modelMatrix);

    /* Render the obstacle cars, considering the changes in position and direction */
    for (int i = 0; i < number_of_obstacles; i++) {
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3D::Translate(obstacle_pos[i].x, obstacle_pos[i].y + 0.101f, obstacle_pos[i].z);
        modelMatrix *= transform3D::RotateOY(-obstacle_dir_angle[i]);
        modelMatrix *= transform3D::Scale(0.3f, 0.3f, 0.3f);
        modelMatrix *= transform3D::RotateOY(AI_MATH_HALF_PI);
        RenderMesh(meshes["car"], shaders["MyShader"], modelMatrix);
    }

    /* Render the fence */
    {
        RenderMesh(meshes["fence"], shaders["MyShader"], transform3D::RotateOY(AI_MATH_HALF_PI) * transform3D::Translate(6, 0, -2.4f) * transform3D::Scale(0.6, 0.6, 0.6));
        RenderMesh(meshes["fence"], shaders["MyShader"], transform3D::RotateOY(AI_MATH_HALF_PI) * transform3D::Translate(3, 0, -2.4f) * transform3D::Scale(0.6, 0.6, 0.6));
        RenderMesh(meshes["fence"], shaders["MyShader"], transform3D::RotateOY(AI_MATH_HALF_PI) * transform3D::Translate(0, 0, -2.4f) * transform3D::Scale(0.6, 0.6, 0.6));

        RenderMesh(meshes["fence"], shaders["MyShader"], transform3D::RotateOY(AI_MATH_HALF_PI) * transform3D::Translate(6, 0, 2.4f) * transform3D::Scale(0.6, 0.6, 0.6));
        RenderMesh(meshes["fence"], shaders["MyShader"], transform3D::RotateOY(AI_MATH_HALF_PI) * transform3D::Translate(3, 0, 2.4f) * transform3D::Scale(0.6, 0.6, 0.6));
        RenderMesh(meshes["fence"], shaders["MyShader"], transform3D::RotateOY(AI_MATH_HALF_PI) * transform3D::Translate(0, 0, 2.4f) * transform3D::Scale(0.6, 0.6, 0.6));
    }

    /* Render the snow */
    glPointSize(1);
    RenderMesh(meshes["snow"], shaders["VertexColor"], transform3D::Translate(-150, snow1_tr_down, -150));
    RenderMesh(meshes["snow"], shaders["VertexColor"], transform3D::Translate(-144, snow2_tr_down, -153));
    glPointSize(5);

}

void Tema2::Update(float deltaTimeSeconds)
{
    /* SHADER VAL 0.01 */
    glUseProgram(shaders["MyShader"]->program);
    glUniform3fv(carPosLoc, 1, glm::value_ptr(car_pos));

    /* World Properties */
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    
    /* Third Person Camera Update */
    /* idee - joaca te cu 0 1 0 de mai jos pt mai multe unghiuri de camera */
    FOV = 85 + car_speed * 1.5f;
    camera = GetSceneCamera();
    camera->SetPerspective(FOV, window->props.aspectRatio, 0.1f, 200.f);
    camera_pos = car_pos - car_dir * 1.5f + glm::vec3(0, 1, 0);
    camera_dir = glm::quatLookAt(glm::normalize(car_pos + car_dir - camera_pos + glm::vec3{0, .5f, 0}), glm::vec3(0, 1, 0));
    camera->SetPositionAndRotation(camera_pos, camera_dir);
    camera->Update();
    RenderScene();

    /* Minimap Camera Update */
    if (minimap_state) {
        glClear(GL_DEPTH_BUFFER_BIT);
        glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);
        camera = GetSceneCamera();
        camera->SetOrthographic(20, 20, 0.1f, 100.f);
        camera_pos = car_pos + glm::vec3(0, 5, 0);
        camera_dir = glm::quatLookAt(glm::vec3(0, -1, 0), car_dir);
        camera->SetPositionAndRotation(camera_pos, camera_dir);
        camera->Update();
        RenderScene();
    }
    
    /* Main Car Physics */
    car_dir = glm::vec3(cos(car_dir_angle), 0, sin(car_dir_angle));
    car_pos = car_pos + car_dir * car_speed * deltaTimeSeconds;
    if (car_going_forward) {
        if (car_speed >= 0.01f) {
            car_speed -= 0.01f;
        }
        else if (car_speed > 0) {
            car_speed = 0;
        }

        if (car_speed > 10) {
            car_speed = 10;
        }
    }
    else {
        if (car_speed <= -0.01f) {
            car_speed += 0.01f;
        }
        else if (car_speed < 0) {
            car_speed = 0;
        }

        if (car_speed < -4) {
            car_speed = -4;
        }
    }

    

    /* Main Car Collision with exterior of the road */
    glm::vec3 closest_segment;
    glm::vec3 closest_point;
    if (!CarIsOnTrack(car_pos, closest_segment, closest_point, 4)) {
        /* Angle between closest segment and car dir */
        float temp_cos = glm::dot(closest_segment, car_dir) / (glm::length(closest_segment) * glm::length(car_dir));
        if (temp_cos > 0.95f) {
            /* In order to avoid infinite collision when car_dir parralel with road */
            car_pos = closest_point;
        }

        if (car_going_forward) {
            car_going_forward = false;
            car_speed = -car_speed * 0.6;
            car_pos = car_pos - car_dir;
        }
        else {
            car_going_forward = true;
            car_speed = -car_speed * 0.6;
            car_pos = car_pos + car_dir;
        }
    }


    /* Obstacle Cars Motion */
    for (int i = 0; i < number_of_obstacles; i++) {
        if (CarIsOnTrack(obstacle_pos[i], closest_segment, closest_point, 2)) {
            if (i < number_of_obstacles / 2 + 1) {
                closest_segment = -closest_segment;
            }

            obstacle_dir[i] = glm::normalize(closest_segment);
            obstacle_dir_angle[i] = atan2(closest_segment.z, closest_segment.x);
            obstacle_pos[i] = obstacle_pos[i] + obstacle_dir[i] * obstacle_speed[i] * deltaTimeSeconds;
        }
        else {
            obstacle_pos[i] += glm::normalize(closest_point - obstacle_pos[i]) * obstacle_speed[i] * deltaTimeSeconds;
        }


        if (obstacle_speed[i] > 5) {
            obstacle_speed[i] = 5;
        }
        obstacle_speed[i] += deltaTimeSeconds * 2;
    }


    /* Collisions - Main With Obstacle Cars */
    for (int i = 0; i < number_of_obstacles; i++) {

        /* Collision Detected */
        if (glm::length(obstacle_pos[i] - car_pos) <= 0.7f) {

            /* Angle between main car and obstacle car directions */
            float temp_cos = glm::dot(obstacle_dir[i], car_dir) / (glm::length(obstacle_dir[i]) * glm::length(car_dir));
            if (temp_cos > 0.8f) {
                /* Main car hits obstacle car from behind */
                if (car_speed > obstacle_speed[i]) {
                    car_going_forward = false;
                    car_speed = -car_speed * 0.6;
                    car_pos = car_pos - car_dir;

                    obstacle_speed[i] = obstacle_speed[i] * 2;
                    obstacle_pos[i] = obstacle_pos[i] + obstacle_dir[i] * 2.f;
                    //obstacle_pos[i] = obstacle_pos[i] + car_dir * 2.f;
                }
                /* Obstacle car hits main from behind */
                else {
                    car_going_forward = true;
                    if (abs(car_speed) < 2.f) {
                        car_speed += 4;
                    }
                    else {
                        car_speed = abs(car_speed) * 0.6;
                    }
                    car_pos = car_pos + car_dir;

                    obstacle_speed[i] = -obstacle_speed[i] * 0.6;
                    obstacle_pos[i] = obstacle_pos[i] - obstacle_dir[i];
                }
            }
            /* Frontal impact */
            else if (temp_cos < -0.8f) {
                car_going_forward = false;
                car_speed = -car_speed * 0.6;
                if (car_speed < 2.f) {
                    car_speed = -3.f;
                }
                car_pos = car_pos - car_dir;

                obstacle_speed[i] = -obstacle_speed[i] * 0.6;
                obstacle_pos[i] = obstacle_pos[i] - obstacle_dir[i];
            }
            /* Lateral Imapact */
            else {
                /* Obstacle -> Main  */
                if (car_speed < obstacle_speed[i]) {
                    car_pos = car_pos + obstacle_dir[i] * 2.f;
                    obstacle_speed[i] = -obstacle_speed[i] * 0.6;
                    obstacle_pos[i] = obstacle_pos[i] - obstacle_dir[i];
                }
                /* Main -> Obstacle */
                else {
                    car_going_forward = false;
                    car_speed = -car_speed * 0.6;
                    car_pos = car_pos - car_dir;

                    obstacle_speed[i] = -obstacle_speed[i] * 0.6;
                    obstacle_pos[i] = obstacle_pos[i] + car_dir;
                }
                
            }
        }
    }

    /* Collisions - Obstacle Cars with themselves */
    for (int i = 0; i < number_of_obstacles - 1; i++) {
        for (int j = i + 1; j < number_of_obstacles; j++) {

            /* Collision Detected */
            if (glm::length(obstacle_pos[i] - obstacle_pos[j]) <= 0.7f) {
                /* Angle between two obstacle cars */
                float temp_cos = glm::dot(obstacle_dir[i], obstacle_dir[j]) / (glm::length(obstacle_dir[i]) * glm::length(obstacle_dir[j]));
                /* Same direction */
                if (temp_cos > 0.8f) {
                    obstacle_pos[i] = obstacle_pos[i] + 1.5f * obstacle_dir[i];

                    obstacle_speed[j] = 0;
                    obstacle_pos[j] = obstacle_pos[j] - 1.5f * obstacle_dir[j];
                /* Frontal imapct */
                } else if (temp_cos < -0.8f) {

                    obstacle_pos[i] = obstacle_pos[i] - glm::cross(glm::normalize(obstacle_dir[i]), glm::vec3(0, 1, 0));
                    obstacle_pos[j] = obstacle_pos[j] - glm::cross(glm::normalize(obstacle_dir[j]), glm::vec3(0, 1, 0));

                    /*obstacle_pos[i] = obstacle_pos[i] - 2.f * glm::cross(glm::normalize(obstacle_dir[i]), glm::vec3(0, 1, 0));
                    obstacle_pos[j] = obstacle_pos[j] - 2.f * obstacle_dir[j];*/

                    //obstacle_pos[i] = obstacle_pos[i] - 2.f * obstacle_dir[j];
                    /*obstacle_pos[j] = obstacle_pos[j] - 2.f * obstacle_dir[i];*/

                    obstacle_speed[i] = -obstacle_speed[i] * 0.6;
                    obstacle_speed[j] = -obstacle_speed[j] * 0.6;
                }
            }
        }
    }

    /* Continuous Snow Effect */
    //snow_translate_down -= 5 * deltaTimeSeconds;
    snow1_tr_down -= 5 * deltaTimeSeconds;
    snow2_tr_down -= 5 * deltaTimeSeconds;

    if (snow1_tr_down <= -104) snow1_tr_down = 96;
    if (snow2_tr_down <= -104) snow2_tr_down = 96;

}


void Tema2::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    if (window->KeyHold(GLFW_KEY_W)) {
        /* Accelerate */
        car_speed += 0.08f;
        if (car_speed > 0) {
            car_going_forward = true;
        }
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        if (car_speed <= 0) {
            /* Go reverse */
            car_going_forward = false;
            car_speed -= 0.03f;
        } else {
            /* Brake */
            car_speed -= 0.04f;
        }      
    }

    /* Turn sideways only when abs(speed) > 1 */
    if (abs(car_speed) > 1.f) {
        if (window->KeyHold(GLFW_KEY_A)) {
            if (car_going_forward) {
                car_dir_angle -= deltaTime;
                car_speed -= 0.01f;
            }
            else {
                car_dir_angle += deltaTime;
                car_speed += 0.01f;
            }
            
        }
        if (window->KeyHold(GLFW_KEY_D)) {
            if (car_going_forward) {
                car_dir_angle += deltaTime;
                car_speed -= 0.01f;
            }
            else {
                car_dir_angle -= deltaTime;
                car_speed += 0.01f;
            }
        }
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
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

    if (key == GLFW_KEY_M) {
        if (minimap_state == false) {
            minimap_state = true;
        }
        else {
            minimap_state = false;
        }
    }
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
