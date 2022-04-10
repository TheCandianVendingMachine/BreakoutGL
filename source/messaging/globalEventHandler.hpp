// globalEventHandler.hpp
// A specialised case for events that reach across global scope: events can be signalled by anyone
#pragma once
#include "eventSource.hpp"

class globalEventHandler : public eventSource
	{
		public:
			void signal(message m);
	};

namespace globals
	{
		extern globalEventHandler *g_globalEventHandler;
	}
