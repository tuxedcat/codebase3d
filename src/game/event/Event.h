namespace evt{
	class Event{
	public:
	};

	class KeyboardInput: public Event{

	};

	class MouseMove: public Event{
	public:
		float x, y; //relative ratio from top-left corner
		MouseMove(float x, float y):x(x),y(y){}
	};

	class MouseDown: public Event{

	};
}