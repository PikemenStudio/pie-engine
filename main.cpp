//
// Created by Full-Hat on 22/11/2023.
//

#include <iostream>

#include "Core/Engine.hpp"
#include "Core/Instance.hpp"

int main() {
    Engine eng({1000, 1000}, "engine");
    eng.RunMainCycle();
}
