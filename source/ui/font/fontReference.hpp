// fontReference.hpp
// A handle-like object to refer to fonts in engine
#pragma once
#include "font.hpp"
#include <string>

class fontEngine;
class fontReference
    {
        public:
            using fontID = int;

        private:
            fontEngine *m_engineReference = nullptr;
            fontID m_fontReference = 0;

            fontReference(fontEngine *engine, fontID font) :
                m_engineReference(engine),
                m_fontReference(font)
            {
            }

            friend class fontEngine;

        public:
            font &font() const;
            operator ::font&() const;

            std::string id();

            static fontReference &null()
                {
                    static fontReference nullFont(nullptr, 0);
                    return nullFont;
                }
    };

