#include "inputHandler.hpp"
#include "iniConfig.hpp"
#include "str.hpp"
#include <GLFW/glfw3.h>

inputHandler *globals::g_inputs = nullptr;

inputHandler::inputHandler(GLFWwindow *window) :
    m_window(window)
    {
    }

inputHandler::inputHandler(GLFWwindow *window, const char *inputFilePath) :
    m_window(window)
    {
        load(inputFilePath);
    }

void inputHandler::load(const char *filepath)
    {
        m_inputGroups = m_defaultInputGroups;
        m_inputs = m_defaultInputs;

        iniConfig config(filepath);
        for (const auto &pair : config.sections)
            {
                for (const auto &[key, keyCode] : pair.second.keyValuePairs)
                    {
                        addInput(pair.first, key, keyCode);
                    }
            }
    }

void inputHandler::save(const char *filepath) const
    {
        iniConfig config;

        for (auto &[group, keys] : m_inputGroups)
            {
                for (auto &key : keys)
                    {
                        int keyCode = m_inputs.at(FE_STR(group.data())).at(FE_STR(key.c_str()));
                        config[group][key] = keyCode;
                    }
            }

        for (auto &[group, keys] : m_defaultInputGroups)
            {
                for (auto &key : keys)
                    {
                        int keyCode = m_defaultInputs.at(FE_STR(group.data())).at(FE_STR(key.c_str()));
                        if (m_inputs.find(FE_STR(group.data())) == m_inputs.end() ||
                            m_inputs.at(FE_STR(group.data())).find(FE_STR(key.c_str())) == m_inputs.at(FE_STR(group.data())).end()
                            ) 
                            {
                                config[group][key] = keyCode;
                            }
                    }
            }

        config.save(filepath);
    }

inputHandler::input inputHandler::addInput(std::string_view group, std::string_view keyName, int keyCode)
    {
        inputHandler::input newInput;
        newInput.m_group = FE_STR(group.data());
        newInput.m_keyName = FE_STR(keyName.data());
        newInput.m_keyCode = keyCode;

        m_inputGroups[group.data()].push_back(keyName.data());
        m_inputs[newInput.group][newInput.keyName] = keyCode;

        return newInput;
    }

inputHandler::input inputHandler::addDefaultInput(std::string_view group, std::string_view keyName, int keyCode)
    {
        inputHandler::input newInput;
        newInput.m_group = FE_STR(group.data());
        newInput.m_keyName = FE_STR(keyName.data());
        newInput.m_keyCode = keyCode;

        m_defaultInputGroups[group.data()].push_back(keyName.data());
        m_defaultInputs[newInput.m_group][newInput.m_keyName] = keyCode;

        return newInput;
    }

int inputHandler::inputCode(std::string_view group, std::string_view keyName) const
    {
        fe::str groupStr = FE_STR(group.data());
        fe::str keyStr = FE_STR(keyName.data());

        if (m_inputs.find(groupStr) == m_inputs.end() ||
            m_inputs.at(groupStr).find(keyStr) == m_inputs.at(groupStr).end()
            )
            {
                return defaultInputCode(group, keyName);
            }
        return m_inputs.at(groupStr).at(keyStr);
    }

int inputHandler::inputCode(inputHandler::input input) const
    {
        if (m_inputs.find(input.group) == m_inputs.end() ||
            m_inputs.at(input.group).find(input.keyName) == m_inputs.at(input.group).end()
            )
            {
                return defaultInputCode(input);
            }
        return m_inputs.at(input.group).at(input.keyName);
    }

int inputHandler::defaultInputCode(std::string_view group, std::string_view keyName) const
    {
        fe::str groupStr = FE_STR(group.data());
        fe::str keyStr = FE_STR(keyName.data());

        if (m_defaultInputs.find(groupStr) == m_defaultInputs.end() ||
            m_defaultInputs.at(groupStr).find(keyStr) == m_defaultInputs.at(groupStr).end()
            )
            {
                return -1;
            }
        return m_defaultInputs.at(groupStr).at(keyStr);
    }

int inputHandler::defaultInputCode(inputHandler::input input) const
    {
        if (m_defaultInputs.find(input.group) == m_defaultInputs.end() ||
            m_defaultInputs.at(input.group).find(input.keyName) == m_defaultInputs.at(input.group).end()
            )
            {
                return -1;
            }
        return m_defaultInputs.at(input.group).at(input.keyName);
    }

inputHandler::inputState inputHandler::keyState(std::string_view group, std::string_view keyName) const
    {
        return keyState(inputCode(group, keyName));
    }

inputHandler::inputState inputHandler::keyState(int keyCode) const
    {
        constexpr inputState states[2] = {
            inputState::RELEASE,
            inputState::PRESS
        };

        int state = glfwGetKey(const_cast<GLFWwindow *>(m_window), keyCode);
        return states[state];
    }

inputHandler::inputState inputHandler::keyState(inputHandler::input input) const
    {
        return keyState(input.keyCode);
    }

inputHandler::inputState inputHandler::mouseState(std::string_view group, std::string_view mouseName) const
    {
        return mouseState(inputCode(group, mouseName));
    }

inputHandler::inputState inputHandler::mouseState(int mouseCode) const
    {
        constexpr inputState states[2] = {
            inputState::RELEASE,
            inputState::PRESS
        };

        int state = glfwGetMouseButton(const_cast<GLFWwindow*>(m_window), mouseCode);
        return states[state];
    }

inputHandler::inputState inputHandler::mouseState(inputHandler::input input) const
    {
        return mouseState(input.keyCode);
    }

glm::vec2 inputHandler::getCursorPosition() const
    {
        double x, y;
        glfwGetCursorPos(const_cast<GLFWwindow*>(m_window), &x, &y);
        return glm::vec2(x, y);
    }
