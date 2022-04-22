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
				
				glm::vec2 position = currentPosition.top() + top.widget->transform.getPosition(m_windowSize);
				currentPosition.pop();

				for (auto &child : top.children)
					{
						currentPosition.push(position);
						nodesToCheck.push(child);
					}

				glm::vec2 topLeftPosition = glm::vec2(transform * glm::vec4(0.f, 0.f, 0.f, 1.f));
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
								else
									{
										// signal single click
										signal(state.widget.onClickEvent);
									}

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

				glm::vec2 position = currentPosition.top() + top.widget->transform.getPosition(m_windowSize);

				currentPosition.pop();

				for (auto &child : top.children)
					{
						currentPosition.push(position);
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

		widgetState& a = *m_widgets.emplace();
		
		widgetState &b0 = *m_widgets.emplace();
		widgetState &b1 = *m_widgets.emplace();
		widgetState &b2 = *m_widgets.emplace();
		widgetState &b3 = *m_widgets.emplace();

		a.widget.texture = nineBox("9box.png", 8);
		b0.widget.texture = nineBox("9box.png", 8);
		b1.widget.texture = nineBox("9box.png", 8);
		b2.widget.texture = nineBox("9box.png", 8);
		b3.widget.texture = nineBox("9box.png", 8);

		auto &an = m_widgetGraph.addWidget(a.widget, &a);
		auto &bn0 = m_widgetGraph.addWidget(b0.widget, &b0);
		auto &bn1 = m_widgetGraph.addWidget(b1.widget, &b1);
		auto &bn2 = m_widgetGraph.addWidget(b2.widget, &b2);
		auto &bn3 = m_widgetGraph.addWidget(b3.widget, &b3);

		m_widgetGraph.addChild(bn0, bn1);
		m_widgetGraph.addChild(bn0, bn2);
		m_widgetGraph.addChild(bn1, bn3);

		a.widget.transform.setPosition({ 150.f, 150.f }, widgetTransform::type::ABSOLUTE);
		a.widget.transform.setSize({ 300.f, 200.f }, widgetTransform::type::ABSOLUTE);

		b0.widget.transform.setAnchor(widgetTransform::anchor::MIDDLE_RIGHT);
		b0.widget.transform.setPosition({ 1.f, 0.5f }, widgetTransform::type::PERCENT); b0.widget.transform.setSize({100.f, 100.f}, widgetTransform::type::ABSOLUTE);
		b1.widget.transform.setPosition({ -50.f, 100.f }, widgetTransform::type::ABSOLUTE); b1.widget.transform.setSize({ 50.f, 100.f }, widgetTransform::type::ABSOLUTE);
		b2.widget.transform.setPosition({ 0.f, 100.f }, widgetTransform::type::ABSOLUTE); b2.widget.transform.setSize({ 100.f, 50.f }, widgetTransform::type::ABSOLUTE);

		b3.widget.transform.setAnchor(widgetTransform::anchor::TOP_LEFT);
		b3.widget.transform.setPosition({ 0.f, 150.f }, widgetTransform::type::ABSOLUTE); b3.widget.transform.setSize({ 0.1f, 0.05f }, widgetTransform::type::PERCENT);
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
