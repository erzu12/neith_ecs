#pragma once

#include <functional>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <chrono>

class Component {};

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
    std::unordered_map<const std::type_info*, IComeponentVector*> componentMap;
    std::vector<size_t> componentTypes;
    size_t id;
    int entityCount = 0;

    Archetype() {
    }

    template <class T>
    std::vector<T> &getComponentVector() {
        static std::vector<T> componentVector = static_cast<ComponentVector<T>*>(componentMap[&typeid(T)])->getVector();
        return componentVector;
    }

    template <class... ComponentTypes>
    std::tuple<std::vector<ComponentTypes>*...> &getComponentVectors() {
        static std::tuple<std::vector<ComponentTypes>*...> componentVectors = std::make_tuple(&getComponentVector<ComponentTypes>()...);
        return componentVectors;
    }

    template<class... ComponentTypes>
    bool hasComponents() {
        return ((std::find(componentTypes.begin(), componentTypes.end(), typeid(ComponentTypes).hash_code()) != componentTypes.end()) && ...);
    }

    template <class... ComponentTypes>
    size_t addComponent(size_t entityIndex, ComponentTypes... components) {
        entityCount++;
        (getComponentVector<ComponentTypes>().push_back(components), ...);
        return entityCount - 1;
    }

    template <class T>
    void addComponentVector() {
        componentMap[&typeid(T)] = new ComponentVector<T>();
    }
};

template <class... ComponentTypes>
class Archetype2 {
    std::tuple<std::vector<ComponentTypes>...> components;
public:
    int entityCount = 0;
    
    template <class T>
    std::vector<T> &getComponentVector() {
        return std::get<T>()>(components);
    }

    void addComponent(EntityHandle entityHandle, ComponentTypes... components) {
        entityCount++;
        (getComponentVector<ComponentTypes>().push_back(components), ...);
    }
};

class IQuery {
};

template <class... ComponentTypes>
class Query : public IQuery {
public:
    std::vector<size_t> componentTypes;
    std::vector<Archetype*> archetypes;
    std::vector<std::pair<Archetype*, size_t>> *entityMap;
    std::vector<std::tuple<std::vector<ComponentTypes>*...>> mComps;

    Query(std::vector<Archetype*> archetypes, std::vector<std::pair<Archetype*, size_t>> *entityMap) {
        componentTypes = {typeid(ComponentTypes).hash_code()...};
        this->archetypes = archetypes;
        this->entityMap = entityMap;
        for (auto &archetype : archetypes) {
            this->mComps.push_back(std::make_tuple(&archetype->getComponentVector<ComponentTypes>()...));
        };
    }

    template <class Func>
    void get(EntityHandle entity, Func func) {
        //for(auto &comp : mComps) {
        //}
        //func(&std::get<getTypeIndexInTemplateList<ComponentTypes, ComponentTypes...>()>(comp)->at(entityMap->at(entity).second)...);
    }

    template <class Func>
    void each(Func func) {
        for (auto &comp : mComps) {
            size_t entityCount = std::get<0>(comp)->size();
            for (int i = 0; i < entityCount; i++) {
                func(&std::get<getTypeIndexInTemplateList<ComponentTypes, ComponentTypes...>()>(comp)->at(i)...);
            }
        }
    }
};


class Ecs {
private:
    std::vector<std::pair<Archetype*, size_t>> entityMap;
    std::unordered_map<size_t, Archetype*> archetypeMap;
    std::unordered_map<size_t, std::vector<Archetype*>> archetypeWithComponentMap;
    std::unordered_map<size_t, IQuery*> queryMap;
    EntityHandle entityCount = 0;
    Archetype *archetype;

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
        entityMap.push_back(std::make_pair(archetype, archetype->entityCount - 1));
        return entityHandle;
    }

    template <class T>
    void printVec(std::vector<T> &vec, std::string name) {
        std::cout << name << ": ";
        for (auto &v : vec) {
            std::cout << v << ", ";
        }
        std::cout << std::endl;
    }

    template <class... ComponentTypes, class Func>
    void get(EntityHandle entityHandle, Func func) {
        //Archetype *archetype = entityMap[entityHandle].first;
        //if (!archetype->hasComponents<ComponentTypes...>()) {
            //return;
        //}
        //size_t index = entityMap[entityHandle].second;
        //static std::tuple<std::vector<ComponentTypes>*...> comps = std::make_tuple(&archetype->getComponentVector<ComponentTypes>()...);
        //std::tuple<std::vector<ComponentTypes>*...> comps = archetype->getComponentVectors<ComponentTypes...>();
        func(&std::get<getTypeIndexInTemplateList<ComponentTypes, ComponentTypes...>()>(archetype->getComponentVectors<ComponentTypes...>())->at(entityHandle)...);
    }

    template <class... ComponentTypes>
    void createQuery() {
        size_t key = (typeid(ComponentTypes).hash_code() + ...);
        // seems a bit ugly but it works (I think)
        std::vector<std::vector<Archetype*>*> archetypeWithComponentMaps = {&archetypeWithComponentMap[typeid(ComponentTypes).hash_code()]...};
        std::vector<Archetype*> currentMap = *archetypeWithComponentMaps[0];
        std::sort(currentMap.begin(), currentMap.end());
        std::vector<Archetype*> *intersection = new std::vector<Archetype*>();
        for(int i = 1; i < archetypeWithComponentMaps.size(); i++) {
            std::vector<Archetype*>* nextMap = archetypeWithComponentMaps[i];
            std::sort(nextMap->begin(), nextMap->end());
            intersection->clear();
            std::set_intersection(currentMap.begin(), currentMap.end(), nextMap->begin(), nextMap->end(), std::back_inserter(*intersection));
            currentMap = *intersection;
        }
        queryMap[key] = new Query<ComponentTypes...>(currentMap, &entityMap);

    }

    template <class... ComponentTypes>
    Query<ComponentTypes...>* query() {
        size_t key = (typeid(ComponentTypes).hash_code() + ...);
        if (queryMap.find(key) == queryMap.end()) {
            createQuery<ComponentTypes...>();
        }
        IQuery *query = queryMap[key];
        return static_cast<Query<ComponentTypes...>*>(query);
    }

private:
    template <class... ComponentTypes>
    void createArchetype(size_t key) {
        Archetype *archetype = new Archetype();
        archetype->componentTypes = {typeid(ComponentTypes).hash_code()...};
        (archetype->addComponentVector<ComponentTypes>(), ...);
        archetypeMap[key] = archetype;
        (archetypeWithComponentMap[typeid(ComponentTypes).hash_code()].push_back(archetype), ...);
        this->archetype = archetype;
    }
};
