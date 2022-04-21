// widget.hpp
// Defines any widget. A manger class operates on this data to generate UI at runtime
#pragma once
#include "widgetTypes.hpp"
#include "widgetTransform.hpp"

struct widget
	{
		widgetTypes type = widgetTypes::NONE;
		widgetTransform transform;
	};
