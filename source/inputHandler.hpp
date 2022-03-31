// inputHandler.hpp
// Allows the mapping of arbitrary keys to inputs
#pragma once
#include "typeDefines.hpp"
#include <plf_colony.h>
#include <glm/vec2.hpp>
#include <vector>
#include <string>
#include <string_view>
#include <robin_hood.h>

struct GLFWwindow;
class inputHandler
    {
        public:
            struct input
                {
                    const fe::str &group = m_group;
                    const fe::str &keyName = m_keyName;
                    const int &keyCode = m_keyCode;

                    input(const input &rhs) { *this = rhs; };
                    input(input &&rhs) noexcept { *this = std::move(rhs); };
                    input &operator=(const input &rhs)
                        {
                            if (&rhs != this)
                                {
                                    m_group = rhs.group;
                                    m_keyName = rhs.keyName;
                                    m_keyCode = rhs.keyCode;
                                }

                            return *this;
                        }
                    input &operator=(input &&rhs) noexcept
                        {
                            if (&rhs != this)
                                {
                                    m_group = std::move(rhs.m_group);
                                    m_keyName = std::move(rhs.m_keyName);
                                    m_keyCode = std::move(rhs.m_keyCode);
                                }

                            return *this;
                        }

                    private:
                        fe::str m_group = 0;
                        fe::str m_keyName = 0;
                        int m_keyCode = 0;

                        input() {}
                        friend class inputHandler;
                };


        private:
            const GLFWwindow *m_window = nullptr;

            plf::colony<input> m_allInputs;

            robin_hood::unordered_map<std::string, std::vector<std::string>> m_defaultInputGroups;
            robin_hood::unordered_map<fe::str, robin_hood::unordered_map<fe::str, int>> m_defaultInputs;

            robin_hood::unordered_map<std::string, std::vector<std::string>> m_inputGroups;
            robin_hood::unordered_map<fe::str, robin_hood::unordered_map<fe::str, int>> m_inputs;

        public:
            enum class inputState
                {
                    RELEASE,
                    PRESS,
                };

            inputHandler(GLFWwindow *window);
            inputHandler(GLFWwindow *window, const char *inputFilePath);

            void load(const char *filepath);
            void save(const char *filepath) const;

            input addInput(std::string_view group, std::string_view keyName, int keyCode);
            input addDefaultInput(std::string_view group, std::string_view keyName, int keyCode);

            int inputCode(std::string_view group, std::string_view keyName) const;
            int inputCode(inputHandler::input input) const;

            int defaultInputCode(std::string_view group, std::string_view keyName) const;
            int defaultInputCode(inputHandler::input input) const;

            inputState keyState(std::string_view group, std::string_view keyName) const;
            inputState keyState(int keyCode) const;
            inputState keyState(inputHandler::input input) const;

            inputState mouseState(std::string_view group, std::string_view mouseName) const;
            inputState mouseState(int mouseCode) const;
            inputState mouseState(inputHandler::input input) const;

            glm::vec2 getCursorPosition() const;
    };

namespace globals
    {
        extern inputHandler *g_inputs;
    }
