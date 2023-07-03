#include <list>
#include <queue>
#include <functional>
#include "game/event/Event.h"

namespace evt{
	class Manager{
	public:
		static void addHandler(const std::function<void(const MouseMove &)>& handler){
			mouseMoveHandlers.push_back(handler);
		}
		static void pushEvent(const MouseMove& evt){
			mouseMoveQ.emplace(evt);
		}
		static void patchEvents(){
			while(mouseMoveQ.size()>0){
				for(auto&listener:mouseMoveHandlers)
					listener(mouseMoveQ.front());
				mouseMoveQ.pop();
			}
		}
	private:
		static std::list<std::function<void(const MouseMove &)>> mouseMoveHandlers;
		static std::queue<MouseMove> mouseMoveQ;
	};
}