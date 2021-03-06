#include "entity.hpp"
#include "str.hpp"
#include "component.hpp"
#include <spdlog/spdlog.h>

void entity::setTag(const char *tag)
    {
        setTag(FE_STR(tag));
    }

void entity::setTag(fe::str tag)
    {
        m_tags.insert(tag);
    }

bool entity::hasTag(const char *tag) const
    {
        return hasTag(FE_STR(tag));
    }

bool entity::hasTag(fe::str tag) const
    {
#ifdef _DEBUG
        std::string tagName = fe::impl::g_debugStrings.strs.at(tag);
#endif
        return m_tags.find(tag) != m_tags.end();
    }

component &entity::addComponent(component &component)
    {
        if (hasComponent(component.hashedName()))
            {
#if _DEBUG
                spdlog::warn("Adding component to entity which already possesses component [Entity: {} Hash: {}, Real: {}]", name, component.hashedName(), fe::impl::g_debugStrings.strs.at(component.hashedName()));
#else
                spdlog::warn("Adding component to entity which already possesses component [Entity: {} Hash: {}]", name, component.hashedName());
#endif
            }

        m_components.insert({ component.hashedName(), &component});
        component.entity = this;
        return component;
    }

bool entity::hasComponent(const char *name) const
    {
        return hasComponent(FE_STR(name));
    }

bool entity::hasComponent(fe::str name) const
    {
        return m_components.find(name) != m_components.end();
    }

component *entity::getComponent(const char *name) const
    {
        return getComponent(FE_STR(name));
    }

component *entity::getComponent(fe::str name) const
    {
        if (!hasComponent(name))
            {
                return nullptr;
            }

        return m_components.at(name);
    }

void entity::removeComponent(const char *name)
    {
        removeComponent(FE_STR(name));
    }

void entity::removeComponent(fe::str name)
    {
        if (hasComponent(name))
            {
                m_components.at(name)->destroy = true;
                m_components.erase(name);
            }
    }

void entity::kill()
    {
        m_kill = true;
    }

bool entity::killed() const
    {
        return m_kill;
    }

entity::~entity()
    {
        for (auto &component : m_components)
            {
                component.second->destroy = true;
            }
    }
