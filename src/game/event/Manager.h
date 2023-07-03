#include <list>
#include <queue>
#include <functional>
#include "game/event/Event.h"

namespace evt{
	template<class E>
	class Manager{
	public:
		static void addHandler(const std::function<void(const E &)>& handler){
			mouseMoveHandlers.push_back(handler);
		}
		static void pushEvent(const E& evt){
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
		static std::list<std::function<void(const E &)>> mouseMoveHandlers;
		static std::queue<E> mouseMoveQ;
	};
	template<class E>
	std::list<std::function<void(const E &)>> Manager<E>::mouseMoveHandlers;
	template<class E>
	std::queue<E> Manager<E>::mouseMoveQ;
}