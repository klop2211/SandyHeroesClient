#pragma once

class Object;

class Scene
{
public:
	~Scene();


protected:
	std::list<Object*> object_list_;


};

