#ifndef RESOURCEMANAGER_HPP_
#define RESOURCEMANAGER_HPP_

#include <SFML/Graphics.hpp>
#include <map>
#include "Graphics/model.hpp"

extern std::string dataDir;
extern std::string modelDir;
extern std::string fontDir;
extern std::string imgDir;
extern std::string defaultFont;
class ResourceManager{
private:
	std::map<std::string,Model> models;
	std::map<std::string,sf::Font> fonts;
	std::map<std::string,sf::Image> images;
	std::map<std::string,GLuint> texs;

	std::map<std::string, Model> noGLModels;
public:
	Model *loadModel(std::string name);
	sf::Font *loadFont(std::string name);
	sf::Image *loadImage(std::string name);
	GLuint loadTexture(std::string name);

	Model *loadNoGLModel(std::string name);
};

#endif
