// widget.hpp
// Defines any widget. A manger class operates on this data to generate UI at runtime
#pragma once
#include "widgetTypes.hpp"
#include "widgetTransform.hpp"
#include "nineBox.hpp"
#include "typeDefines.hpp"
#include <string>

struct widget
    {
        std::string name;

        widgetTypes type = widgetTypes::NONE;
        widgetTransform transform;
        nineBox texture;

        fe::str onMouseEnterEvent = 0;
        fe::str onMouseLeaveEvent = 0;
        fe::str onClickEvent = 0;
        fe::str onClickStartEvent = 0;
        fe::str onClickEndEvent = 0;
        fe::str onDoubleClickEvent = 0;
        fe::str onDrawEvent = 0;

        bool noMouseInteraction = false;
    };
