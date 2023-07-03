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

	enum class MouseButtonType{
		left,
		right,
	};
	class MousePress: public Event{
	public:
		MouseButtonType btnType;
		MousePress(MouseButtonType btnType):btnType(btnType){}
	};

	class MouseRelease: public Event{
	public:
		MouseButtonType btnType;
		MouseRelease(MouseButtonType btnType):btnType(btnType){}
	};
}