#ifndef RENDERMANAGER_HPP_
#define RENDERMANAGER_HPP_

#include "model.hpp"
#include "GameObject.hpp"
#include <map>

class RenderManager{
private:
	std::vector<GameObject> drawList;
public:
	void render();
};


#endif
