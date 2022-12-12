#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace meshes
{
    Mesh* CreateCar();

    std::pair<Mesh*, Mesh*> CreateRoad(float width, glm::vec3 color1, glm::vec3 color2);

    Mesh* CreateTerrain(int size, std::vector<glm::vec3> colors);

    Mesh* CreateTree();

    Mesh* CreateForest(int size, float density, Mesh* road);

    Mesh* CreateFence();

    Mesh* CreateSnow();
}
