// widget.hpp
// Defines any widget. A manger class operates on this data to generate UI at runtime
#pragma once
#include "widgetTypes.hpp"
#include "widgetTransform.hpp"
#include "nineBox.hpp"
#include "typeDefines.hpp"

struct widget
	{
		widgetTypes type = widgetTypes::NONE;
		widgetTransform transform;
		nineBox texture;

		fe::str onMouseOverEvent = 0;
		fe::str onMouseLeaveEvent = 0;
		fe::str onClickEvent = 0;
		fe::str onDoubleClickEvent = 0;
		fe::str onMouseWheelUpEvent = 0;
		fe::str onMouseWheelDownEvent = 0;

		bool noMouseInteraction = false;
	};
