#include "eventSource.hpp"
#include "str.hpp"

void eventSource::signal(message event)
    {
        for (auto &queuedObserverToDestroy : m_queuedDestruction)
            {
                if (m_observers.find(queuedObserverToDestroy.event) != m_observers.end())
                    {
                        std::vector<observer> &observers = m_observers.at(queuedObserverToDestroy.event);
                        observers.erase(std::remove_if(observers.begin(), observers.end(), [queuedObserverToDestroy] (const observer &test) { return test.id == queuedObserverToDestroy.id; }), observers.end());
                    }
            }

        if (m_observers.find(event.event) != m_observers.end())
            {
                for (auto &observer : m_observers.at(event.event))
                    {
                        observer.callback(event, observer.id);
                    }
            }
    }

int eventSource::subscribe(const char *event, handler callback)
    {
        return subscribe(FE_STR(event), callback);
    }

int eventSource::subscribe(fe::str event, handler callback)
    {
        observer o;
        o.callback = callback;
        o.id = ++m_ids[event];

        m_observers[event].push_back(o);

        return o.id;
    }

void eventSource::unsubscribe(const char *event, int id)
    {
        unsubscribe(FE_STR(event), id);
    }

void eventSource::unsubscribe(fe::str event, int id)
    {
        m_queuedDestruction.push_back(toDestroy{ .event = event, .id = id });
    }
