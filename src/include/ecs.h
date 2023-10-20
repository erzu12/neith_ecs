#pragma once

#include <algorithm>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

class Component {};

//typedef unsigned long long EntityHandle;

class IArchetype {
public:
    size_t entityCount = 0;
    virtual void *getComponentVector(std::type_index) = 0;
};

template<class... ComponentTypes>
class Archetype : public IArchetype {
    std::unordered_map<std::type_index, void*> componentMap;
public:
    std::tuple<std::vector<ComponentTypes>...> components;

    Archetype() {
        components = std::make_tuple(std::vector<ComponentTypes>()...);
        ((componentMap[std::type_index(typeid(ComponentTypes))] = &std::get<std::vector<ComponentTypes>>(components)), ...);
    }

    //template <class T>
    void *getComponentVector(std::type_index componentID) override {
        return componentMap[componentID];
    }

    //std::tuple<std::vector<ComponentTypes>...> *getComponentVectors() {
        //return &components;
    //}

    void addComponent(ComponentTypes... entityComponents) {
        entityCount++;
        (std::get<std::vector<ComponentTypes>>(components).push_back(entityComponents), ...);
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
        this->archetypes = archetypes;
    }

    template <class Func>
    void get(EntityHandle entity, Func func) {
        static IArchetype *lastArchetype = nullptr;
        static std::tuple<std::vector<ComponentTypes>*...> componentVectors =
                std::make_tuple(static_cast<std::vector<ComponentTypes>*>(
                entity.archetype->getComponentVector(std::type_index(typeid(ComponentTypes))))...);
        if (lastArchetype != entity.archetype) {
            componentVectors = std::make_tuple(static_cast<std::vector<ComponentTypes>*>(
                    entity.archetype->getComponentVector(std::type_index(typeid(ComponentTypes))))...);
            lastArchetype = entity.archetype;
        }
        func(&std::get<std::vector<ComponentTypes>*>(componentVectors)->at(entity.index)...);
    }

    template <class Func>
    void each(Func func) {
        for (auto &archetype : archetypes) {
            std::tuple<std::vector<ComponentTypes>*...> componentVectors =
                    std::make_tuple(static_cast<std::vector<ComponentTypes>*>(
                    archetype->getComponentVector(std::type_index(typeid(ComponentTypes))))...);

            for (int i = 0; i < archetype->entityCount; i++) {
                func(&std::get<std::vector<ComponentTypes>*>(componentVectors)->at(i)...);
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
        archetypeMap[key] = archetype;
        (archetypeWithComponentMap[typeid(ComponentTypes).hash_code()].push_back(archetype), ...);
    }
};
