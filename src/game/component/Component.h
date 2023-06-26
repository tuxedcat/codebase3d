#pragma once
struct Entity;
struct Component{
public:
	Component(Entity* owner=nullptr):owner(owner){}
private:
	Entity* owner;
};