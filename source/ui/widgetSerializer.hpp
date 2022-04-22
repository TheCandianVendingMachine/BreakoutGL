// widgetSerializer.hpp
// Handles the serialization of widgets into/out of disk
#pragma once

class widgetManager;
namespace widgetSerializer
	{
		void saveToFile(widgetManager& widgets, const char *file);
		void loadFromFile(widgetManager& widgets, const char *file);
	};
