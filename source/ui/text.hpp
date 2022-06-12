// text.hpp
// Defines text as a UI element. Can be modified during runtime
#pragma once
#include <string>
#include "font/fontReference.hpp"
#include "graphics/vertexArray.hpp"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

class widgetManager;
namespace widgetSerializer 
    {
        void saveToFile(widgetManager&, const char*);
        void loadFromFile(widgetManager&, const char*);
    };

class text
    {
        private:
            std::string m_string;
            fontReference m_font = fontReference::null();

            glm::vec2 m_size;
            vertexArray m_textVertexArray;
            glm::vec4 m_colour = { 1.f, 1.f, 1.f, 1.f };
            bool m_needsRebuild = true;

            friend void widgetSerializer::saveToFile(widgetManager&, const char*);

        public:
            const bool &needsRebuild = m_needsRebuild;
            const std::string &string = m_string;
            const vertexArray &vertexArray = m_textVertexArray;
            const fontReference &font = m_font;
            const glm::vec2 &size = m_size;

            text() = default;
            text(const text &rhs) = default;
            text &operator=(const text &rhs);

            text(fontReference font);
            text(const char *text, fontReference font);

            void setText(const char *text);
            void setFont(fontReference font);

            void build(glm::vec2 size);
    };

