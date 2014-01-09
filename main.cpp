#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shaderutils.hpp"
#include "model.hpp"
using namespace std;

GLuint vboTriangle;
GLuint vboColour;
GLint attribute_texcoord, attribute_coord3d, attribute_vweight, 
	  attribute_vtangent, attribute_normal,attribute_vbones;
GLint uniform_texture, uniform_mvp, uniform_bonemats;

struct attributes {
	GLfloat coord3d[3];
	GLfloat colour[3];
};

void initData(){
	attribute_coord3d = getAttribute("coord3d");
	attribute_texcoord = getAttribute("texcoord");
	attribute_vweight = getAttribute("vweight");
	//attribute_normal = getAttribute("normal");
	//attribute_vtangent = getAttribute("vtangent");
	attribute_vbones = getAttribute("vbones");
	uniform_mvp = getUniform("mvp");
	uniform_bonemats = getUniform("bonemats");
}

void clearResources(){
	glDeleteProgram(programShader);
	glDeleteBuffers(1,&vboTriangle);
}

int main(int argc, char *argv[]){
	sf::ContextSettings cs;
	cs.majorVersion = 3;
	cs.minorVersion = 0;
	cs.depthBits = 24;
	cs.stencilBits = 8;
	cs.antialiasingLevel = 4;
	int width = 800;
	int height = 600;
	sf::RenderWindow window(sf::VideoMode(width,height),"SPACE!!!", sf::Style::Default, cs);
	sf::Event event;

	GLenum glewStatus = glewInit();
	if(glewStatus != GLEW_OK){
		cerr << "Error: " << glewGetErrorString(glewStatus) << endl;
		return EXIT_FAILURE;
	}

	initResources();
	initData();

	Model mesh;
	loadIQM("mrfixit.iqm",mesh);
	float currentFrame = 0;

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0,0.0,0.0,1.0);

	sf::Clock time;
	time.restart();

	window.setActive(true);

	bool animate = false;
	bool spin = false;
	bool ortho = false;

	glm::vec3 cameraPos = glm::vec3(0.0,10.0,8.0);

	while(window.isOpen()){
		while(window.pollEvent(event)){
			if(event.type == sf::Event::Closed){
				window.close();
			}
			if(event.type == sf::Event::Resized){
				width = event.size.width;
				height = event.size.height;
				glViewport(0,0,width,height);
			}
			if(event.type == sf::Event::KeyPressed) {
				if(event.key.code == sf::Keyboard::Left)
					if(currentFrame>0)
						currentFrame--;
				if(event.key.code == sf::Keyboard::Right)
					currentFrame++;
				if(event.key.code == sf::Keyboard::M)
					animate = !animate;
				if(event.key.code == sf::Keyboard::P)
					spin = !spin;
				if(event.key.code == sf::Keyboard::O)
					ortho = !ortho;
			}
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
			cameraPos.y++;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
			cameraPos.x--;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
			cameraPos.y--;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
			cameraPos.x++;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){
			cameraPos.z--;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)){
			cameraPos.z++;
		}
		//Uncomment this to play the animation normally
		if(animate){
				float timey = time.getElapsedTime().asMilliseconds();
				currentFrame = (timey / 1000.0 * mesh.anims[0].framerate);
				currentFrame = fmod(currentFrame,mesh.numFrames);
		}
		mesh.animate(currentFrame);
		float angle = time.getElapsedTime().asMilliseconds() / 1000.0 * 15;  // base 15° per second
		glm::mat4 anim = \
			glm::rotate(glm::mat4(1.0f), angle*3.0f, glm::vec3(1, 0, 0)) *  // X axis
			glm::rotate(glm::mat4(1.0f), angle*2.0f, glm::vec3(0, 1, 0)) *  // Y axis
			glm::rotate(glm::mat4(1.0f), angle*4.0f, glm::vec3(0, 0, 1));   // Z axis

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -15.0));
		//cameraPos = glm::vec3(0.0,10.0,0.0);
		glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0, 0.0, -15.0), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 projection;
		if(!ortho)
			projection = glm::perspective(45.0f, 1.0f*width/height, 0.1f, 1000.0f);
		else
			projection = glm::ortho<float>(-10.0,10.0,-10.0,10.0,-10,40);
		glm::mat4 mvp = projection * view * model; //* anim;
		if(spin) mvp *= anim;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Do all drawing here
		glUniformMatrix4fv(uniform_mvp,1,GL_FALSE,glm::value_ptr(mvp));

		mesh.draw();

		window.display();
	}

	clearResources();
}
