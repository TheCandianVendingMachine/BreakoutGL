// entity.hpp
// A bag of components. Effectively used as a handle for a bunch of components
#pragma once
#include <robin_hood.h>
#include <string>
#include "typeDefines.hpp"

struct component;
class entity
    {
        private:
            robin_hood::unordered_map<fe::str, component*> m_components;
            robin_hood::unordered_set<fe::str> m_tags;

            bool m_kill = false;

        public:
            const robin_hood::unordered_set<fe::str> &tags = m_tags;
            std::string name = "";

            void setTag(const char *tag);
            void setTag(fe::str tag);

            bool hasTag(const char *tag) const;
            bool hasTag(fe::str tag) const;

            component &addComponent(component &component);
            template<std::derived_from<component> TComponent>
            TComponent &addComponent(component &component);

            bool hasComponent(const char *name) const;
            bool hasComponent(fe::str name) const;

            component *getComponent(const char *name) const;
            component *getComponent(fe::str name) const;

            template<std::derived_from<component> TComponent>
            TComponent *getComponent(const char *name) const;
            template<std::derived_from<component> TComponent>
            TComponent *getComponent(fe::str name) const;

            void removeComponent(const char *name);
            void removeComponent(fe::str name);

            void kill();
            bool killed() const;

            ~entity();

    };

template<std::derived_from<component> TComponent>
TComponent &entity::addComponent(component &component)
    {
        return static_cast<TComponent&>(addComponent(component));
    }

template<std::derived_from<component> TComponent>
TComponent *entity::getComponent(const char *name) const
    {
        return static_cast<TComponent*>(getComponent(name));
    }

template<std::derived_from<component> TComponent>
TComponent *entity::getComponent(fe::str name) const
    {
        return static_cast<TComponent*>(getComponent(name));
    }
