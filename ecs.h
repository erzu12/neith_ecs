#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <chrono>

typedef unsigned long long EntityHandle;

template<typename Target, typename ListHead, typename... ListTails>
constexpr size_t getTypeIndexInTemplateList() {
    if constexpr (std::is_same<Target, ListHead>::value)
        return 0;
    else
        return 1 + getTypeIndexInTemplateList<Target, ListTails...>();
}

class IComeponentVector {
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

class Archetype {
public:
    std::unordered_map<const char*, IComeponentVector*> componentMap;
    std::vector<size_t> componentTypes;
    std::unordered_set<Archetype*> superSetArchetypes;
    size_t id;
    int entityCount = 0;

    Archetype(size_t id);

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
private:
    std::unordered_map<size_t, Archetype*> archetypeMap;
    std::unordered_map<size_t, std::vector<Archetype*>> archetypeWithComponentMap;
    EntityHandle entityCount = 0;

public:
    template <class... ComponentTypes>
    EntityHandle createEntity(ComponentTypes... components) {
        //could potentially go wrong but unlikely
        size_t key = (typeid(ComponentTypes).hash_code() + ...);
        //std::cout << "key: " << key << std::endl;
        if(archetypeMap.find(key) == archetypeMap.end()) {
            createArchetype<ComponentTypes...>(key);
        }
        Archetype *archetype = archetypeMap[key];
        EntityHandle entityHandle = entityCount++;
        archetype->addComponent(entityHandle, components...);
        return entityHandle;
    }

    template <class... ComponentTypes, class Func>
    void eachComponent(Func func) {
        size_t key = (typeid(ComponentTypes).hash_code() + ...);
        Archetype *archetype = this->archetypeMap[key];
        if (archetype == nullptr) {
            std::cout << "no archetype found" << std::endl;
        }
        std::tuple<std::vector<ComponentTypes>*...> comps = std::make_tuple(&archetype->getComponentVector<ComponentTypes>()...);
        auto starttime = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < archetype->entityCount; i++) {
            func(&std::get<getTypeIndexInTemplateList<ComponentTypes, ComponentTypes...>()>(comps)->at(i)...);
        }
        for (auto &superSetArchetype : archetype->superSetArchetypes) {
            std::tuple<std::vector<ComponentTypes>*...> comps = std::make_tuple(&superSetArchetype->getComponentVector<ComponentTypes>()...);
            for (int i = 0; i < superSetArchetype->entityCount; i++) {
                func(&std::get<getTypeIndexInTemplateList<ComponentTypes, ComponentTypes...>()>(comps)->at(i)...);
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "run: " << std::chrono::duration_cast<std::chrono::microseconds>(end - starttime).count() << std::endl;
    }

private:
    template <class... ComponentTypes>
    void createArchetype(size_t key) {
        std::cout << "key: " << key << std::endl;
        Archetype *archetype = new Archetype(key);
        archetype->componentTypes = {typeid(ComponentTypes).hash_code()...};
        (archetype->addComponentVector<ComponentTypes>(), ...);
        archetypeMap[key] = archetype;
        setSubAndSuperSetArchetypest(archetype);
        (archetypeWithComponentMap[typeid(ComponentTypes).hash_code()].push_back(archetype), ...);
    }

    void setSubAndSuperSetArchetypest(Archetype *newArchetype);

    bool isSubset(std::vector<size_t> subset, std::vector<size_t> set);
};
