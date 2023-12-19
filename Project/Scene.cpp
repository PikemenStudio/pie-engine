//
// Created by Full-Hat on 19/12/2023.
//

#include "Scene.hpp"

Scene::Scene() {
    for (float x = -1.0f; x < 1.0f; x += 0.2f) {
        for (float y = -1.0f; y < 1.0f; y += 0.2f) {
            m_positions.push_back({x, y, 0.0f});
        }
    }
}
