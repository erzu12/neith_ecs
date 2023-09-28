#include <functional>
#include <iostream>

#include <thread>
#include <unordered_set>
#include <vector>
#include <chrono>

#include "ecs.h"

class Component {};

struct Transform : public Component {
    float x, y, z;
    // override <<
    
    Transform(double x, double y, double z) : x(x), y(y), z(z) {}

    friend std::ostream& operator<<(std::ostream& os, const Transform& t) {
        os << "Transform: " << t.x << ", " << t.y << ", " << t.z;
        return os;
    }
};

struct Size : public Component {
    float x, y, z;
    // override <<

    Size(double x, double y, double z) : x(x), y(y), z(z) {}

    friend std::ostream& operator<<(std::ostream& os, const Size& t) {
        os << "Transform: " << t.x << ", " << t.y << ", " << t.z;
        return os;
    }
};

struct Velocity : public Component {
    float x, y, z;
    Velocity(double x, double y, double z) : x(x), y(y), z(z) {}
    friend std::ostream& operator<<(std::ostream& os, const Velocity& t) {
        os << "Velocity: " << t.x << ", " << t.y << ", " << t.z;
        return os;
    }
};



void loop(std::vector<Transform>& v1, std::vector<Velocity>& v2, int count) {
    float total = 0;
    for(int i = 0; i < count; i++) {
        total += v1[i].x;
        total += v2[i].x;
    }
    std::cout << "Total: " << total << std::endl;
}


int main()
{
    Transform transform = Transform{1, 2, 3};
    Velocity velocity = Velocity{4, 5, 6};
    Size size = Size{7, 8, 9};
    std::vector<Transform> v1;
    std::vector<Velocity> v2;
    std::vector<Size> v3;

    int count = 10000000;

    for (int i = 0; i < count; i++) {
        v1.push_back(transform);
        v2.push_back(velocity);
        v3.push_back(size);
    }


    std::cout << "transform: " << transform << std::endl;
    std::cout << "transform vec: " << *static_cast<Transform*>(&v1[0]) << std::endl;

    float total = 0;
    
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;

    Ecs ecs;

    //for(int i = 0; i < count / 2; i++) {
        //ecs.createEntity(v1[i], v2[i]);
    //}
    //for(int i = count / 2; i < count; i++) {
        //ecs.createEntity(v2[i], v3[i], v1[i]);
    //}

    ecs.createEntity(Transform(1, 1, 1));
    ecs.createEntity(Transform(1, 1, 1), Velocity(1, 1, 1));
    ecs.createEntity(Velocity(1, 1, 1), Transform(1, 1, 1));
    ecs.createEntity(Velocity(1, 1, 1), Size(1, 1, 1), Transform(1, 1, 1));
    ecs.createEntity(Size(1, 1, 1), Velocity(1, 1, 1), Transform(1, 1, 1));
    ecs.createEntity(Size(1, 1, 1), Transform(1, 1, 1));


    start = std::chrono::high_resolution_clock::now();
    
    //loop(v1, v2, count);
    
    ecs.filter<Transform, Velocity, Size>()->each([&](Transform* t, Velocity* v, Size* s) {
        total += t->x;
    });
    std::cout << "Total: " << total << std::endl;
    total = 0;

    ecs.filter<Transform, Velocity>()->each([&](Transform* t, Velocity* v) {
        total += t->x;
    });
    std::cout << "Total: " << total << std::endl;
    total = 0;

    ecs.filter<Size, Velocity>()->each([&](Size* s, Velocity* v) {
        total += s->x;
    });
    std::cout << "Total: " << total << std::endl;
    total = 0;

    ecs.filter<Transform, Size>()->each([&](Transform* t, Size* s) {
        total += t->x;
    });
    std::cout << "Total: " << total << std::endl;
    total = 0;

    ecs.filter<Transform>()->each([&](Transform* t) {
        total += t->x;
    });
    std::cout << "Total: " << total << std::endl;
    total = 0;

    ecs.filter<Velocity>()->each([&](Velocity* v) {
        total += v->x;
    });
    std::cout << "Total: " << total << std::endl;
    total = 0;

    ecs.filter<Size>()->each([&](Size* s) {
        total += s->x;
    });

    std::cout << "Total: " << total << std::endl;
    total = 0;

    //ecs.filter<Size>()->each([&](Size* s) {
        //total += s->x;
    //});

    end = std::chrono::high_resolution_clock::now();
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;

    std::cout << "Total: " << total << std::endl;
    
    return 0;
}
