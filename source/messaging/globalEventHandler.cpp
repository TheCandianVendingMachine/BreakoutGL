#include "globalEventHandler.hpp"

globalEventHandler* globals::g_globalEventHandler = nullptr;

void globalEventHandler::signal(message m)
	{
		eventSource::signal(m);
	}
