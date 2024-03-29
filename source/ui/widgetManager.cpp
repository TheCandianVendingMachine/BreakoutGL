#include "widgetManager.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stack>
#include <queue>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>
#include "graphics/camera.hpp"
#include "iniConfig.hpp"
#include "str.hpp"

#undef ABSOLUTE

void widgetManager::traverseRoot(widgetGraph::node &root)
    {
        // DFS on the widget graph. Checks all widgets for mouse collision, and processes on that
        std::stack<widgetGraph::node*> nodesToCheck;
        std::stack<glm::vec2> currentPosition;

        nodesToCheck.push(&root);
        currentPosition.push(glm::vec2(0.f));

        glm::vec2 cursorPosition = globals::g_inputs->getCursorPosition();
        while (!nodesToCheck.empty())
            {
                widgetGraph::node &top = *nodesToCheck.top();
                nodesToCheck.pop();

                glm::mat4 transform = glm::translate(glm::mat4(1.f), glm::vec3(currentPosition.top(), 0.f)) * top.widget->transform.getMatrix(m_windowSize);
                glm::vec2 topLeftPosition = glm::vec2(transform * glm::vec4(0.f, 0.f, 0.f, 1.f));

                currentPosition.pop();

                for (auto &child : top.children)
                    {
                        currentPosition.push(topLeftPosition);
                        nodesToCheck.push(child);
                    }

                if (!top.widget->noMouseInteraction)
                    {
                        glm::vec2 size = glm::vec2(transform * glm::vec4(1.f, 1.f, 0.f, 1.f)) - topLeftPosition;

                        widgetState &state = *static_cast<widgetState*>(top.metaData);
                        if (cursorPosition.x >= topLeftPosition.x && cursorPosition.x <= topLeftPosition.x + size.x &&
                            cursorPosition.y >= topLeftPosition.y && cursorPosition.y <= topLeftPosition.y + size.y)
                            {
                                if (!state.cursorOn)
                                    {
                                        // signal cursor enter
                                        state.cursorOn = true;
                                        signal(FE_STR(state.widget.onMouseEnterEvent.c_str()));
                                    }

                                if (globals::g_inputs->mouseState(m_guiClick) == inputHandler::inputState::PRESS && !state.clicked)
                                    {
                                        // signal click on
                                        signal(FE_STR(state.widget.onClickStartEvent.c_str()));
                                        if (m_guiClock.getTime() - state.lastClicked <= m_doubleClickThreshold)
                                            {
                                                // signal double click
                                                signal(FE_STR(state.widget.onDoubleClickEvent.c_str()));
                                            }
                                        // signal single click
                                        signal(FE_STR(state.widget.onClickEvent.c_str()));

                                        state.clicked = true;
                                        state.lastClicked = m_guiClock.getTime();
                                    }
                                else if (globals::g_inputs->mouseState(m_guiClick) == inputHandler::inputState::RELEASE && state.clicked)
                                    {
                                        // signal click off
                                        signal(FE_STR(state.widget.onClickEndEvent.c_str()));
                                        state.clicked = false;
                                    }
                            }
                        else
                            {
                                if (state.cursorOn)
                                    {
                                        // singlam cursor leave
                                        signal(FE_STR(state.widget.onMouseLeaveEvent.c_str()));
                                        state.cursorOn = false;
                                    }
                            }
                    }
            }
    }

void widgetManager::drawRoot(widgetGraph::node& root)
    {
        // DFS on the widget graph. Render all widgets as we pass them
        std::stack<widgetGraph::node*> nodesToCheck;
        std::stack<glm::vec2> currentPosition;

        nodesToCheck.push(&root);
        currentPosition.push(glm::vec2(0.f));

        struct widgetDrawInfo
            {
                glm::mat4 transform = glm::mat4(1.f);
                widget *widget = nullptr;
                widgetState *state = nullptr;
            };
        std::queue<widgetDrawInfo> toDraw;

        glm::vec2 cursorPosition = globals::g_inputs->getCursorPosition();
        while (!nodesToCheck.empty())
            {
                widgetGraph::node &top = *nodesToCheck.top();
                nodesToCheck.pop();

                glm::mat4 transform = glm::translate(glm::mat4(1.f), glm::vec3(currentPosition.top(), 0.f)) * top.widget->transform.getMatrix(m_windowSize);
                glm::vec2 topLeftPosition = glm::vec2(transform * glm::vec4(0.f, 0.f, 0.f, 1.f));

                currentPosition.pop();

                for (auto &child : top.children)
                    {
                        currentPosition.push(topLeftPosition);
                        nodesToCheck.push(child);
                    }

                toDraw.emplace(transform, top.widget, static_cast<widgetState*>(top.metaData));
            }

        while (!toDraw.empty())
            {
                widgetDrawInfo drawInfo = toDraw.front();
                toDraw.pop();

                signal(FE_STR(drawInfo.state->widget.onDrawEvent.c_str()));

                if (drawInfo.widget->hasText)
                    {
                        if (drawInfo.widget->text.needsRebuild)
                            {
                                drawInfo.widget->text.build(m_windowSize);
                                drawInfo.widget->textTransform.setSize({ 1.f, 1.f }, widgetTransformEnums::type::ABSOLUTE);
                            }

                        // draw
                        glm::mat4 parentTransform = glm::translate(glm::mat4(1.f), glm::vec3(drawInfo.transform[3]));
                        m_widgetShader.setMat4("model", parentTransform * drawInfo.widget->textTransform.getMatrix(m_windowSize));

                        const vertexArray &vertexArray = drawInfo.widget->text.vertexArray;
                        glBindVertexArray(vertexArray.vao);

                        drawInfo.widget->text.font.font().texture.bind(GL_TEXTURE0);
                        glDrawElements(GL_TRIANGLES, vertexArray.indexCount, GL_UNSIGNED_INT, 0);
                    }

                m_widgetShader.setMat4("model", drawInfo.transform);

                const vertexArray &vertexArray = drawInfo.widget->texture.getVertexArray(drawInfo.widget->transform.getSize(m_windowSize));
                glBindVertexArray(vertexArray.vao);

                drawInfo.widget->texture.texture.bind(GL_TEXTURE0);
                glDrawElements(GL_TRIANGLES, vertexArray.indexCount, GL_UNSIGNED_INT, 0);
            }
    }

widgetManager::widgetManager(glm::vec2 windowSize, const char *fontFile) :
    m_guiClick(globals::g_inputs->addDefaultInput("GUI", "Click", GLFW_MOUSE_BUTTON_LEFT)),
    m_windowSize(windowSize),
    m_widgetShader("shaders/widgets.vs", "shaders/widgets.fs")
    {
        iniConfig globalUISettings("gui_settings.ini");
        if (globalUISettings["gui"].has("double click time"))
            {
                m_doubleClickThreshold = fe::seconds(globalUISettings["gui"]["double click time"].asDouble());
            }
        else
            {
                globalUISettings["gui"]["double click time"] = m_doubleClickThreshold.asSeconds();
            }
        globalUISettings.save("gui_settings.ini");

        iniConfig fonts(fontFile);
        for (auto &[fontName, section] : fonts.sections)
            {
                m_fonts.load(section["file"].asString().data(), fontName.c_str(), section["size"].asUnsignedInt());
            }
    }

widgetManager::~widgetManager()
    {
    }

void widgetManager::setWindowSize(glm::vec2 size)
    {
        m_windowSize = size;
    }

void widgetManager::update()
    {
        for (auto &root : m_widgetGraph.roots)
            {
                traverseRoot(*root);
            }
    }

void widgetManager::draw(const camera& camera, unsigned int texture)
    {
        m_widgetShader.use();
        m_widgetShader.setMat4("projection", camera.projection());

        for (auto &root : m_widgetGraph.roots)
            {
                drawRoot(*root);
            }
    }
