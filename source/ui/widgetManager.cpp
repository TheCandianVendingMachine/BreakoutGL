#include "widgetManager.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stack>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>
#include "graphics/camera.hpp"
#include "iniConfig.hpp"

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
										signal(state.widget.onMouseEnterEvent);
									}

								if (globals::g_inputs->mouseState(m_guiClick) == inputHandler::inputState::PRESS && !state.clicked)
									{
										// signal click on
										signal(state.widget.onClickStartEvent);
										if (m_guiClock.getTime() - state.lastClicked <= m_doubleClickThreshold)
											{
												// signal double click
												signal(state.widget.onDoubleClickEvent);
											}
										// signal single click
										signal(state.widget.onClickEvent);

										state.clicked = true;
										state.lastClicked = m_guiClock.getTime();
									}
								else if (globals::g_inputs->mouseState(m_guiClick) == inputHandler::inputState::RELEASE && state.clicked)
									{
										// signal click off
										signal(state.widget.onClickEndEvent);
										state.clicked = false;
									}
							}
						else
							{
								if (state.cursorOn)
									{
										// singlam cursor leave
										signal(state.widget.onMouseLeaveEvent);
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
		std::stack<widgetDrawInfo> toDraw;

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
				widgetDrawInfo drawInfo = toDraw.top();
				toDraw.pop();

				signal(drawInfo.state->widget.onDrawEvent);

				m_widgetShader.setMat4("model", drawInfo.transform);

				const vertexArray &vertexArray = drawInfo.widget->texture.getVertexArray(drawInfo.widget->transform.getSize(m_windowSize));
				glBindVertexArray(vertexArray.vao);

				drawInfo.widget->texture.texture.bind(GL_TEXTURE0);
				glDrawElements(GL_TRIANGLES, vertexArray.indexCount, GL_UNSIGNED_INT, 0);
			}
	}

widgetManager::widgetManager(glm::vec2 windowSize) :
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
