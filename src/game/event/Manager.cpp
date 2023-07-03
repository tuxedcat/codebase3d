#include "game/event/Manager.h"

namespace evt{
	std::list<std::function<void(const MouseMove &)>> Manager::mouseMoveHandlers;
	std::queue<MouseMove> Manager::mouseMoveQ;
}