#include <functional>
#include <iostream>

#include <thread>
#include <unordered_set>
#include <vector>
#include <chrono>

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

template<typename Target, typename ListHead, typename... ListTails>
constexpr size_t getTypeIndexInTemplateList()
{
    if constexpr (std::is_same<Target, ListHead>::value)
        return 0;
    else
        return 1 + getTypeIndexInTemplateList<Target, ListTails...>();
}


template <class... ComponteTyps>
class Archetype {
public:
    //std::vector<std::vector<Component>> components;
    std::tuple<std::vector<ComponteTyps>...> *comps;
    int entityCount = 0;

    void printComponentTypes() {
        std::cout << "Component Types: " << std::endl;
        (std::cout << ... << typeid(ComponteTyps).name()) << std::endl;
    }
    
    template <typename...Types>
    void eachComponent(std::function<void(Types*...args)> func) {
        std::cout << "entityCount: " << entityCount << ", actual: " << std::get<0>(*comps).size() << std::endl;
        for (int i = 0; i < entityCount; i++) {
            func(&std::get<getTypeIndexInTemplateList<Types, ComponteTyps...>()>(*comps).at(i)...);
        }
    }

    //void eachComponent(std::function<void(ComponteTyps*...args)> func) {
        //auto starttime = std::chrono::high_resolution_clock::now();
        //int threadCount = 2;
        //int split = entityCount / threadCount;
        //std::thread threads[threadCount];
        //for (int i = 0; i < threadCount; i++) {
            //auto f = [&](int start) {
                //std::cout << "start: " << start << ", offset: " << threadCount << std::endl;
                //for (int j = start; j < entityCount; j += threadCount) {
                    //func(&std::get<getTypeIndexInTemplateList<ComponteTyps, ComponteTyps...>()>(*comps)[j]...);
                //}
            //};
            //threads[i] = std::thread(f, i);
        //}
        //auto end = std::chrono::high_resolution_clock::now();
        //std::cout << "create thread: " << std::chrono::duration_cast<std::chrono::microseconds>(end - starttime).count() << std::endl;
        //starttime = std::chrono::high_resolution_clock::now();
        //for (int i = 0; i < threadCount; i++) {
            //threads[i].join();
        //}
        //end = std::chrono::high_resolution_clock::now();
        //std::cout << "run: " << std::chrono::duration_cast<std::chrono::microseconds>(end - starttime).count() << std::endl;
    //}

};

//template<class... ComponentTypes>
//class Filter {
//public:
//Ecs *ecs;
//Filter(Ecs *ecs) : ecs(ecs) {
//}

//void eachComponent(std::function<void(ComponentTypes*...args)> func) {
//std::string key = (std::string(typeid(ComponentTypes).name()) + ...);
//Archetype2 *archetype = ecs->archetypeMap[key];
//std::tuple<std::vector<ComponentTypes>*...> comps = std::make_tuple(&archetype->getComponentVector<ComponentTypes>()...);
//auto starttime = std::chrono::high_resolution_clock::now();
//for (int i = 0; i < ecs->entityCount; i++) {
//func(&std::get<getTypeIndexInTemplateList<ComponentTypes, ComponentTypes...>()>(comps)->at(i)...);
//}
//auto end = std::chrono::high_resolution_clock::now();
//std::cout << "run: " << std::chrono::duration_cast<std::chrono::microseconds>(end - starttime).count() << std::endl;
//}

//};

typedef unsigned long long EntityHandle;

class IComeponentVector {
public:
};

template <class T>
class ComponentVector : public IComeponentVector {
public:
    std::vector<T> &getVector() {
        return v;
    }
private:
    std::vector<T> v;
};

class Archetype2 {
public:
    std::unordered_map<const char*, IComeponentVector*> componentMap;
    std::vector<size_t> componentTypes;
    std::unordered_set<Archetype2*> superSetArchetypes;
    size_t id;
    int entityCount = 0;

    Archetype2(size_t id) {
        this->id = id;
    }


    template <class T>
    std::vector<T> &getComponentVector() {
        return static_cast<ComponentVector<T>*>(componentMap[typeid(T).name()])->getVector();
    }

    template <class... ComponentTypes>
    void addComponent(EntityHandle entityHandle, ComponentTypes... components) {
        entityCount++;
        (getComponentVector<ComponentTypes>().push_back(components), ...);
    }

    template <class T>
    void addComponentVector() {
        componentMap[typeid(T).name()] = new ComponentVector<T>();
    }
};

class Ecs {
public:
    std::unordered_map<size_t, Archetype2*> archetypeMap;
    std::unordered_map<size_t, std::vector<Archetype2*>> archetypeWithComponentMap;
    EntityHandle entityCount = 0;

    bool isSubset(std::vector<size_t> subset, std::vector<size_t> set) {
        for (auto &i : subset) {
            if (std::find(set.begin(), set.end(), i) == set.end()) {
                return false;
            }
        }
        return true;
    }

    void setSubAndSuperSetArchetypest(Archetype2 *newArchetype) {
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

    template <class... ComponentTypes>
    EntityHandle createEntity(ComponentTypes... components) {
        //could potentially go wrong but unlikely
        size_t key = (typeid(ComponentTypes).hash_code() + ...);
        //std::cout << "key: " << key << std::endl;
        if(archetypeMap.find(key) == archetypeMap.end()) {
            std::cout << "key: " << key << std::endl;
            Archetype2 *archetype = new Archetype2(key);
            archetype->componentTypes = {typeid(ComponentTypes).hash_code()...};
            archetypeMap[key] = archetype;
            setSubAndSuperSetArchetypest(archetype);
            (archetypeWithComponentMap[typeid(ComponentTypes).hash_code()].push_back(archetype), ...);
            (archetype->addComponentVector<ComponentTypes>(), ...);
        }
        Archetype2 *archetype = archetypeMap[key];
        EntityHandle entityHandle = entityCount++;
        archetype->addComponent(entityHandle, components...);
        return entityHandle;
    }

    template <class... ComponentTypes, class Func>
    void eachComponent(Func func) {
        size_t key = (typeid(ComponentTypes).hash_code() + ...);
        Archetype2 *archetype = this->archetypeMap[key];
        std::tuple<std::vector<ComponentTypes>*...> comps = std::make_tuple(&archetype->getComponentVector<ComponentTypes>()...);
        auto starttime = std::chrono::high_resolution_clock::now();
        std::cout << "entityCount: " << archetype->entityCount << std::endl;
        for (int i = 0; i < archetype->entityCount; i++) {
            func(&std::get<getTypeIndexInTemplateList<ComponentTypes, ComponentTypes...>()>(comps)->at(i)...);
        }
        for (auto &superSetArchetype : archetype->superSetArchetypes) {
            std::cout << "entityCount: " << superSetArchetype->entityCount << std::endl;
            std::tuple<std::vector<ComponentTypes>*...> comps = std::make_tuple(&superSetArchetype->getComponentVector<ComponentTypes>()...);
            for (int i = 0; i < superSetArchetype->entityCount; i++) {
                func(&std::get<getTypeIndexInTemplateList<ComponentTypes, ComponentTypes...>()>(comps)->at(i)...);
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "run: " << std::chrono::duration_cast<std::chrono::microseconds>(end - starttime).count() << std::endl;
    }
};



template <typename... Types> void func2(std::vector<Types>... v2);

int loop(Archetype<Transform, Velocity> *archetype, std::vector<Transform> *v1, std::vector<Velocity> *v2, int total, int count)  {
    for (int i = 0; i < count; i++) {
        Transform t = std::get<0>(*archetype->comps).at(i);
        Velocity v = std::get<1>(*archetype->comps).at(i);
        total += t.x + v.x;
    }
    std::cout << "Total: " << total << std::endl;
    return total;
}

int main()
{
    std::cout << "Hello, World!" << std::endl;

    Archetype<Transform, Velocity> *archetype = new Archetype<Transform, Velocity>();
    Archetype<Transform, Velocity, Size> *archetype2 = new Archetype<Transform, Velocity, Size>();

    Transform transform = Transform{1, 2, 3};
    Velocity velocity = Velocity{4, 5, 6};
    Size size = Size{7, 8, 9};
    std::vector<Transform> v1;
    std::vector<Velocity> v2;
    std::vector<Size> v3;

    int count = 10000000;
    archetype->entityCount = count;
    //archetype2->entityCount = count / 2;

    for (int i = 0; i < count; i++) {
        v1.push_back(transform);
        v2.push_back(velocity);
        v3.push_back(size);
    }


    std::cout << "transform: " << transform << std::endl;
    std::cout << "transform vec: " << *static_cast<Transform*>(&v1[0]) << std::endl;

    //archetype->comps = std::make_tuple(v1, v2);

    archetype->comps = new std::tuple<std::vector<Transform>, std::vector<Velocity>>(v1, v2);
    archetype2->comps = new std::tuple<std::vector<Transform>, std::vector<Velocity>, std::vector<Size>>(v1, v2, v3);


    //archetype.printComponentTypes();
    //archetype.PrintComponents();

    std::cout << "Each Component: " << std::endl;

    float total = 0;

    
    auto start = std::chrono::high_resolution_clock::now();
    loop(archetype, &v1, &v2, total, count);

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;


    //archetype->eachComponent([&](Transform* t, Velocity* v) {
            //total += t->x + v->x;
    //});

    total = 0;


    Ecs ecs;
    for(int i = 0; i < count / 2; i++) {
        ecs.createEntity(v1[i], v2[i]);
    }
    for(int i = count / 2; i < count; i++) {
        ecs.createEntity(v2[i], v3[i], v1[i]);
    }

    //Filter<Transform, Velocity> filter(&ecs);
    start = std::chrono::high_resolution_clock::now();
    ecs.eachComponent<Transform, Velocity>([&](Transform* t, Velocity* v) {
        total += t->x + v->x;
    });

    end = std::chrono::high_resolution_clock::now();
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;

    std::cout << "Total: " << total << std::endl;
    
    return 0;
}
