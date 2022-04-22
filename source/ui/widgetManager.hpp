// widgetManager.hpp
// Owns all widgets and handles all functions on widgets
#pragma once
#include <plf_colony.h>
#include "widget.hpp"
#include "widgetGraph.hpp"
#include "inputHandler.hpp"
#include "graphics/shader.hpp"
#include "time.hpp"
#include "clock.hpp"
#include "messaging/eventSource.hpp"

class widgetManager;
namespace widgetSerializer 
	{
		void saveToFile(widgetManager&, const char*);
		void loadFromFile(widgetManager&, const char*);
	};
class camera;
class widgetManager : public eventSource
	{
		private:
			struct widgetState
				{
					widget widget;
					bool cursorOn = false;
					bool clicked = false;
					fe::time lastClicked = fe::seconds(0);
				};

			fe::clock m_guiClock;
			fe::time m_doubleClickThreshold = fe::milliseconds(500);

			inputHandler::input m_guiClick;

			plf::colony<widgetState> m_widgets;
			widgetGraph m_widgetGraph;

			glm::vec2 m_windowSize = { 0.f, 0.f };

			shader m_widgetShader;

			void traverseRoot(widgetGraph::node &root);
			void drawRoot(widgetGraph::node &root);

			friend void widgetSerializer::saveToFile(widgetManager&, const char*);
			friend void widgetSerializer::loadFromFile(widgetManager&, const char*);

		public:
			widgetManager(glm::vec2 windowSize);
			~widgetManager();

			void setWindowSize(glm::vec2 size);

			void update();
			void draw(const camera &camera, unsigned int texture);
	};
