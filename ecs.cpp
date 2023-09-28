#include "ecs.h"

#include <functional>
#include <iostream>

#include <thread>
#include <chrono>

Archetype::Archetype(size_t id) {
    this->id = id;
}

