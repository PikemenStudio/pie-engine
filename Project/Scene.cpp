//
// Created by Full-Hat on 19/12/2023.
//

#include "Scene.hpp"

Scene::Scene() {
    // Init meshes
    m_meshes = std::make_unique<MeshesManager>();

    // Init scene
    float x = -0.6f;
    for (float y = -1.0f; y < 1.0f; y += 0.2f) {

        trianglePositions.push_back(glm::vec3(x, y, 0.0f));

    }

    x = 0.0f;
    for (float y = -1.0f; y < 1.0f; y += 0.2f) {

        squarePositions.push_back(glm::vec3(x, y, 0.0f));

    }

}
