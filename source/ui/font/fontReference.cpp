#include "fontReference.hpp"
#include "fontEngine.hpp"

font &fontReference::font() const
    {
        if (!m_engineReference)
            {
                return null();
            }
        return m_engineReference->m_allFonts.at(m_engineReference->m_fontIndexLookup.at(m_fontReference));
    }

std::string fontReference::id()
    {
        if (!m_engineReference)
            {
                return "ERROR: NO ENGINE";
            }
        return m_engineReference->m_fontIDLookup.at(m_fontReference);
    }

fontReference::operator::font&() const
    {
        return font();
    }
