#include "ecs.h"

#include <functional>
#include <iostream>

#include <thread>
#include <chrono>

Archetype::Archetype(size_t id) {
    this->id = id;
}


void Ecs::setSubAndSuperSetArchetypest(Archetype *newArchetype) {
    std::vector<size_t> componentTypes = newArchetype->componentTypes;
    for (auto &componentType : componentTypes) {
        for (auto &archetype : archetypeWithComponentMap[componentType]) {
            if(isSubset(archetype->componentTypes, componentTypes)) {
                std::cout << "archetype: " << newArchetype->id << "is super set of: " << archetype->id << std::endl;
                archetype->superSetArchetypes.emplace(newArchetype);
            }
            else if(isSubset(componentTypes, archetype->componentTypes)) {
                std::cout << "archetype: " << archetype->id << "is super set of: " << newArchetype->id << std::endl;
                newArchetype->superSetArchetypes.emplace(archetype);
            }
        }
    }
}

bool Ecs::isSubset(std::vector<size_t> subset, std::vector<size_t> set) {
    for (auto &i : subset) {
        if (std::find(set.begin(), set.end(), i) == set.end()) {
            return false;
        }
    }
    return true;
}
