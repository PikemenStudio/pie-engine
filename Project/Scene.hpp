//
// Created by Full-Hat on 19/12/2023.
//

#pragma once

#include <ranges>

#include "glm/vec3.hpp"

class Scene {
public:
    Scene();

public:
    std::vector<glm::vec3> m_positions;
};
