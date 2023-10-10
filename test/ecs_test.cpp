#include <gtest/gtest.h>

#include <ecs.h>

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

TEST(EcsTest, createEntity) {
    Ecs ecs;
    auto entity = ecs.createEntity(Transform(1, 2, 3));
    EXPECT_EQ(entity, 0);
}

TEST(EcsTest, createEntityWithMultipleComponents) {
    Ecs ecs;
    auto entity = ecs.createEntity(Transform(1, 2, 3), Velocity(4, 5, 6));
    EXPECT_EQ(entity, 0);
}

TEST(EcsTest, createMultipleEntities) {
    Ecs ecs;
    auto entity1 = ecs.createEntity(Transform(1, 2, 3), Velocity(4, 5, 6));
    auto entity2 = ecs.createEntity(Transform(1, 2, 3), Velocity(4, 5, 6));
    EXPECT_EQ(entity1, 0);
    EXPECT_EQ(entity2, 1);
}

TEST(EcsTest, query) {
    Ecs ecs;
    auto entity = ecs.createEntity(Transform(1, 2, 3), Velocity(4, 5, 6));
    ecs.query<Transform, Velocity>()->each([&](Transform *t, Velocity *v) {
        EXPECT_EQ(t->x, 1);
        EXPECT_EQ(t->y, 2);
        EXPECT_EQ(t->z, 3);
        EXPECT_EQ(v->x, 4);
        EXPECT_EQ(v->y, 5);
        EXPECT_EQ(v->z, 6);
    });
}

TEST(EcsTest, querySubsetOfComponents) {
    Ecs ecs;
    auto entity = ecs.createEntity(Transform(1, 2, 3), Velocity(4, 5, 6));
    ecs.query<Transform>()->each([&](Transform *t) {
        EXPECT_EQ(t->x, 1);
        EXPECT_EQ(t->y, 2);
        EXPECT_EQ(t->z, 3);
    });
    ecs.query<Velocity>()->each([&](Velocity *v) {
        EXPECT_EQ(v->x, 4);
        EXPECT_EQ(v->y, 5);
        EXPECT_EQ(v->z, 6);
    });
}


TEST(EcsTest, querySuppersetOfComponents) {
    Ecs ecs;
    auto entity = ecs.createEntity(Transform(1, 2, 3), Velocity(4, 5, 6));
    ecs.query<Transform, Velocity, Size>()->each([&](Transform *t, Velocity *v, Size *s) {
        FAIL();
    });
}

TEST(EcsTest, query2of3Components) {
    Ecs ecs;
    ecs.createEntity(Transform(1, 1, 1), Velocity(1, 1, 1), Size(1, 1, 1));
    ecs.createEntity(Transform(1, 1, 1), Velocity(1, 1, 1));
    int total = 0;
    ecs.query<Transform, Velocity>()->each([&](Transform *t, Velocity *v) {
        total += t->x;
    });
    EXPECT_EQ(total, 2);
}

TEST(EcsTest, query3Components) {
    Ecs ecs;
    auto entity = ecs.createEntity(Transform(1, 2, 3), Velocity(4, 5, 6), Size(7, 8, 9));
    int total = 0;
    ecs.query<Transform, Velocity, Size>()->each([&](Transform *t, Velocity *v, Size *s) {
        total += t->x;
    });
    EXPECT_EQ(total, 1);
}

TEST(EcsTest, get) {
    Ecs ecs;
    auto entity0 = ecs.createEntity(Transform(1, 1, 1));
    auto entity1 = ecs.createEntity(Transform(2, 2, 2));
    auto entity2 = ecs.createEntity(Transform(3, 3, 3));
    ecs.get<Transform>(entity1, [&](Transform *t) {
        EXPECT_EQ(t->x, 2);
    });
    ecs.get<Transform>(entity2, [&](Transform *t) {
        EXPECT_EQ(t->x, 3);
    });
    ecs.get<Transform>(entity0, [&](Transform *t) {
        EXPECT_EQ(t->x, 1);
    });
}

TEST(EcsTest, getDifferentComps) {
    Ecs ecs;
    auto entity0 = ecs.createEntity(Transform(1, 1, 1));
    auto entity1 = ecs.createEntity(Transform(2, 2, 2), Velocity(2, 2, 2));
    auto entity2 = ecs.createEntity(Velocity(3, 3, 3));

    ecs.get<Transform>(entity0, [&](Transform *t) {
        EXPECT_EQ(t->x, 1);
    });

    ecs.get<Transform>(entity1, [&](Transform *t) {
        EXPECT_EQ(t->x, 2);
    });

    ecs.get<Velocity>(entity1, [&](Velocity *v) {
        EXPECT_EQ(v->x, 2);
    });
}

TEST(EcsTest, getInvalidTypes) {
    Ecs ecs;
    auto entity0 = ecs.createEntity(Transform(1, 1, 1));
    auto entity2 = ecs.createEntity(Velocity(3, 3, 3));

    ecs.get<Transform>(entity2, [&](Transform *t) {
        FAIL() << "Should not get here";
    });

    ecs.get<Velocity>(entity0, [&](Velocity *v) {
        FAIL() << "Should not get here";
    });
}



TEST(EcsTest, complete) {
    Ecs ecs;
    ecs.createEntity(Transform(1, 1, 1));
    ecs.createEntity(Transform(1, 1, 1), Velocity(1, 1, 1));
    ecs.createEntity(Velocity(1, 1, 1), Transform(1, 1, 1));
    ecs.createEntity(Velocity(1, 1, 1), Size(1, 1, 1), Transform(1, 1, 1));
    ecs.createEntity(Size(1, 1, 1), Velocity(1, 1, 1), Transform(1, 1, 1));
    ecs.createEntity(Size(1, 1, 1), Transform(1, 1, 1));


    long total = 0;

    ecs.query<Transform, Velocity, Size>()->each([&](Transform* t, Velocity* v, Size* s) {
        total += t->x;
    });
    
    EXPECT_EQ(total, 2);
    total = 0;

    ecs.query<Transform, Velocity>()->each([&](Transform* t, Velocity* v) {
        total += t->x;
    });

    EXPECT_EQ(total, 4);
    total = 0;

    ecs.query<Size, Velocity>()->each([&](Size* s, Velocity* v) {
        total += s->x;
    });

    EXPECT_EQ(total, 2);
    total = 0;

    ecs.query<Transform, Size>()->each([&](Transform* t, Size* s) {
        total += t->x;
    });

    EXPECT_EQ(total, 3);
    total = 0;

    ecs.query<Transform>()->each([&](Transform* t) {
        total += t->x;
    });
    EXPECT_EQ(total, 6);
    total = 0;

    ecs.query<Velocity>()->each([&](Velocity* v) {
        total += v->x;
    });
    EXPECT_EQ(total, 4);
    total = 0;

    ecs.query<Size>()->each([&](Size* s) {
        total += s->x;
    });
    EXPECT_EQ(total, 3);
    total = 0;
}



