//
// Created by FullHat on 13/05/2024.
//

#include "../facades/facade.hpp"

template <SceneManagerDependenciesConcept Dep>
scene_manager_facades::SceneManagerBaseImpl<Dep>::SceneManagerBaseImpl(
    SceneManagerFacadeStructs::SceneManagerProps<Dep> &&Props) {}

template <SceneManagerDependenciesConcept Dep>
scene_manager_facades::SceneManagerBaseImpl<Dep>::~SceneManagerBaseImpl() {}

template class scene_manager_facades::SceneManagerBaseImpl<
    scene_manager_facades::SceneManagerDependencies>;
