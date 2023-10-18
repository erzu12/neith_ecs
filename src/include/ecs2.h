#pragma once

#include <algorithm>
#include <any>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

class Component {};

//typedef unsigned long long EntityHandle;

class IArchetype {
};

template<class... ComponentTypes>
class Archetype : public IArchetype {
    std::tuple<std::vector<ComponentTypes>...> components;
    public:
    int entityCount = 0;

    //template <class T>
    std::vector<std::any> *getComponentVector() {
        return std::get<T>()>(components);
    }

    //std::tuple<std::vector<ComponentTypes>...> *getComponentVectors() {
        //return &components;
    //}

    void addComponent(ComponentTypes... components) {
        entityCount++;
        (getComponentVector<ComponentTypes>().push_back(components), ...);
    }
};

typedef struct EntityHandle {
    IArchetype *archetype;
    size_t index;
} EntityHandle;

class IQuery {
};

template <class... ComponentTypes>
class Query : public IQuery {
    std::vector<IArchetype*> archetypes;
public:

    Query(std::vector<IArchetype*> archetypes) {
        //componentTypes = {typeid(ComponentTypes).hash_code()...};
        this->archetypes = archetypes;
    }

    template <class Func>
    void get(EntityHandle entity, Func func) {
    }

    template <class Func>
    void each(Func func) {
        for (auto &archetype : archetypes) {
            std::tuple<std::vector<ComponentTypes>...> *componentVectors = archetype->getComponentVector();
            for (int i = 0; i < entityCount; i++) {
                func(&std::get<getTypeIndexInTemplateList<ComponentTypes, ComponentTypes...>()>(comp)->at(i)...);
            }
        }
    }
};

class Ecs {
private:
    std::unordered_map<size_t, IQuery*> queryMap;
    std::unordered_map<size_t, IArchetype*> archetypeMap;
    std::unordered_map<size_t, std::vector<IArchetype*>> archetypeWithComponentMap;

public:


    template <class... ComponentTypes>
    EntityHandle createEntity(ComponentTypes... components) {
        size_t key = (typeid(ComponentTypes).hash_code() + ...);
        //std::cout << "key: " << key << std::endl;
        if(archetypeMap.find(key) == archetypeMap.end()) {
            createArchetype<ComponentTypes...>(key);
        }
        Archetype<ComponentTypes...> *archetype = static_cast<Archetype<ComponentTypes...>*>(archetypeMap[key]);
        //EntityHandle entityHandle = entityCount++;
        archetype->addComponent(components...);
        //entityMap.push_back(std::make_pair(archetype, archetype->entityCount - 1));
        EntityHandle entityHandle{archetype, archetype->entityCount - 1};
        return entityHandle;
    }


    template <class... ComponentTypes, class Func>
    void get(EntityHandle entityHandle, Func func) {
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
    void createQuery() {
        size_t key = (typeid(ComponentTypes).hash_code() + ...);
        // seems a bit ugly but it works (I think)
        std::vector<std::vector<IArchetype*>*> archetypeWithComponentMaps = {&archetypeWithComponentMap[typeid(ComponentTypes).hash_code()]...};
        std::vector<IArchetype*> currentMap = *archetypeWithComponentMaps[0];
        std::sort(currentMap.begin(), currentMap.end());
        std::vector<IArchetype*> *intersection = new std::vector<IArchetype*>();
        for(int i = 1; i < archetypeWithComponentMaps.size(); i++) {
            std::vector<IArchetype*>* nextMap = archetypeWithComponentMaps[i];
            std::sort(nextMap->begin(), nextMap->end());
            intersection->clear();
            std::set_intersection(currentMap.begin(), currentMap.end(), nextMap->begin(), nextMap->end(), std::back_inserter(*intersection));
            currentMap = *intersection;
        }
        queryMap[key] = new Query<ComponentTypes...>(currentMap);
    }

    template <class... ComponentTypes>
    void createArchetype(size_t key) {
        IArchetype *archetype = new Archetype<ComponentTypes...>();
        //archetypeMap[key] = archetype;
        (archetypeWithComponentMap[typeid(ComponentTypes).hash_code()].push_back(archetype), ...);
    }
};
