#include "widgetManager.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stack>
#include <glm/mat4x4.hpp>
#include <spdlog/spdlog.h>
#include "graphics/camera.hpp"

#undef ABSOLUTE

void widgetManager::traverseRoot(widgetGraph::node &root)
	{
		// DFS on the widget graph. Checks all widgets for mouse collision, and processes on that
		std::stack<widgetGraph::node*> nodesToCheck;
		std::stack<glm::mat4> currentTransform;

		nodesToCheck.push(&root);
		currentTransform.push(glm::mat4(1.f));

		glm::vec2 cursorPosition = globals::g_inputs->getCursorPosition();
		while (!nodesToCheck.empty())
			{
				widgetGraph::node &top = *nodesToCheck.top();
				nodesToCheck.pop();

				glm::mat4 transform = root.widget->transform.getMatrix(m_windowSize) * currentTransform.top();
				currentTransform.pop();

				for (auto &child : top.children)
					{
						currentTransform.push(transform);
						nodesToCheck.push(child);
					}

				glm::vec2 position = transform * glm::vec4(0.f, 0.f, 0.f, 1.f);
				glm::vec2 size = glm::vec2(transform * glm::vec4(1.f, 1.f, 0.f, 1.f)) - position;

				if (cursorPosition.x >= position.x && cursorPosition.x <= position.x + size.x &&
					cursorPosition.y >= position.y && cursorPosition.y <= position.y + size.y)
					{
						// intersects
						spdlog::info("Intersects");
					}
			}
	}

void widgetManager::drawRoot(widgetGraph::node& root)
	{
		// DFS on the widget graph. Render all widgets as we pass them
		std::stack<widgetGraph::node*> nodesToCheck;
		std::stack<glm::mat4> currentTransform;

		nodesToCheck.push(&root);
		currentTransform.push(glm::mat4(1.f));

		glm::vec2 cursorPosition = globals::g_inputs->getCursorPosition();
		while (!nodesToCheck.empty())
			{
				widgetGraph::node &top = *nodesToCheck.top();
				nodesToCheck.pop();

				glm::mat4 transform = root.widget->transform.getMatrix(m_windowSize) * currentTransform.top();
				currentTransform.pop();

				for (auto &child : top.children)
					{
						currentTransform.push(transform);
						nodesToCheck.push(child);
					}

				m_widgetShader.setMat4("model", transform);

				const vertexArray &vertexArray = top.widget->texture.getVertexArray(top.widget->transform.getSize(m_windowSize));
				glBindVertexArray(vertexArray.vao);

				top.widget->texture.texture.bind(GL_TEXTURE0);
				glDrawElements(GL_TRIANGLES, vertexArray.indexCount, GL_UNSIGNED_INT, 0);
			}
	}

widgetManager::widgetManager(glm::vec2 windowSize) :
	m_guiClick(globals::g_inputs->addDefaultInput("GUI", "Click", GLFW_MOUSE_BUTTON_LEFT)),
	m_windowSize(windowSize),
	m_widgetShader("shaders/widgets.vs", "shaders/widgets.fs")
	{
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

		//auto &an = m_widgetGraph.addWidget(a.widget);
		auto &bn0 = m_widgetGraph.addWidget(b0.widget);
		auto &bn1 = m_widgetGraph.addWidget(b1.widget);
		//auto &bn2 = m_widgetGraph.addWidget(b2.widget);
		//auto &bn3 = m_widgetGraph.addWidget(b3.widget);

		m_widgetGraph.addChild(bn0, bn1);
		//m_widgetGraph.addChild(bn0, bn2);
		//m_widgetGraph.addChild(bn1, bn3);

		a.widget.transform.setPosition({ 150.f, 150.f }, widgetTransform::type::ABSOLUTE);
		a.widget.transform.setSize({ 300.f, 200.f }, widgetTransform::type::ABSOLUTE);

		b0.widget.transform.setPosition({ 0.5f, 0.5f }, widgetTransform::type::PERCENT); b0.widget.transform.setSize({100.f, 100.f}, widgetTransform::type::ABSOLUTE);
		b1.widget.transform.setPosition({ 50.f, 0.f }, widgetTransform::type::ABSOLUTE); b1.widget.transform.setSize({ 50.f, 100.f }, widgetTransform::type::ABSOLUTE);
		b2.widget.transform.setPosition({ 0.f, 100.f }, widgetTransform::type::ABSOLUTE); b2.widget.transform.setSize({ 100.f, 50.f }, widgetTransform::type::ABSOLUTE);
		b3.widget.transform.setPosition({ 0.f, 5.f }, widgetTransform::type::ABSOLUTE); b3.widget.transform.setSize({ 0.1f, 0.05f }, widgetTransform::type::PERCENT);
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
