// threadJob.hpp
// A abstract struct that allows the user to create custom jobs
#pragma once
#include <functional>

namespace fe
    {
        struct threadJob
            {   
                // Runs a function that the thread executes
                // If this returns false, the thread will run again
                virtual bool execute() = 0;
                bool m_done = false;
                bool m_active = false;
                bool m_started = false;
                bool isDone() const { return m_done || !m_active; }
            };

        struct threadFunction : threadJob
            {
                std::function<bool()> m_function;

                threadFunction() : m_function([]() { return true; }) {}
                threadFunction(std::function<bool()> function) : m_function(function) {}
                bool execute() { return m_function(); }
            };
    }