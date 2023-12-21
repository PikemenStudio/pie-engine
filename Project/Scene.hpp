//
// Created by Full-Hat on 19/12/2023.
//

#pragma once

#include "Project/MeshesManager.hpp"

#include <ranges>

#include "glm/vec3.hpp"

class Scene {
public:
    Scene();
    ~Scene() = default;

public:
    std::vector<glm::vec3> trianglePositions;

    std::vector<glm::vec3> squarePositions;

    MeshesManager& GetMeshes() { return *m_meshes; }
    void SetMeshes(std::unique_ptr<MeshesManager> meshes) { m_meshes = std::move(meshes); }

    void DestroyMeshesSystem() { m_meshes.reset(); }

protected:
    std::unique_ptr<MeshesManager> m_meshes;
};
