// widget.hpp
// Defines any widget. A manger class operates on this data to generate UI at runtime
#pragma once
#include "widgetTypes.hpp"
#include "widgetTransform.hpp"
#include "nineBox.hpp"
#include "text.hpp"
#include <string>

struct widget
    {
        std::string name = "";

        widgetTypes type = widgetTypes::NONE;
        widgetTransform transform;
        nineBox texture;

        bool hasText = false;
        text text;
        widgetTransform textTransform;
        std::string fontID = "";

        std::string onMouseEnterEvent = "";
        std::string onMouseLeaveEvent = "";
        std::string onClickEvent = "";
        std::string onClickStartEvent = "";
        std::string onClickEndEvent = "";
        std::string onDoubleClickEvent = "";
        std::string onDrawEvent = "";

        bool noMouseInteraction = false;
    };
