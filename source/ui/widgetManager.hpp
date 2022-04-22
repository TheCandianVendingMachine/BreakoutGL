// widgetManager.hpp
// Owns all widgets and handles all functions on widgets
#pragma once
#include <plf_colony.h>
#include "widget.hpp"
#include "widgetGraph.hpp"
#include "inputHandler.hpp"
#include "graphics/shader.hpp"

class camera;
class widgetManager
	{
		private:
			struct widgetState
				{
					widget widget;
				};

			inputHandler::input m_guiClick;

			plf::colony<widgetState> m_widgets;
			widgetGraph m_widgetGraph;

			glm::vec2 m_windowSize = { 0.f, 0.f };

			shader m_widgetShader;

			void traverseRoot(widgetGraph::node &root);
			void drawRoot(widgetGraph::node &root);

		public:
			widgetManager(glm::vec2 windowSize);
			~widgetManager();

			void setWindowSize(glm::vec2 size);

			void update();
			void draw(const camera &camera, unsigned int texture);
	};
