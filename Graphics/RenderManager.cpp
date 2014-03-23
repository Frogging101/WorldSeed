#include <SFML/Graphics/Image.hpp>
#include "RenderManager.hpp"
#include "../globals.hpp"

RenderManager::~RenderManager(){
	for(int i=0;i<this->drawList.size();i++){
		delete drawList[i];
	}
}

void RenderManager::renderDepth(ShaderProgram *prg, float dt){
	glm::mat4 view = glm::lookAt(glm::vec3(-4, 6, -4), glm::vec3(0,0,0), 
			glm::vec3(0, 1, 0));
	glm::mat4 proj = glm::ortho<float>(-10.0f,10.0f,-10.0f,10.0f,-10.0f,20.0f);

	glm::mat4 depthMVP = proj*view;
	glUniformMatrix4fv(prg->getUniform("pv"), 1, GL_FALSE, glm::value_ptr(depthMVP));

	glViewport(0,0,1024,1024);
	glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	for(int i=0;i<this->drawList.size();i++){
		if(drawList[i]->animate){
			drawList[i]->aTime += dt; 
			drawList[i]->model->animate(drawList[i]->currentAnimation,
					drawList[i]->aTime,&drawList[i]->outframe);
		}

		glm::mat4 scale = glm::scale(glm::mat4(1),drawList[i]->scale);
		glm::mat4 rot = \
			glm::rotate(glm::mat4(1),drawList[i]->rotation.x,glm::vec3(1.0,0,0)) *
			glm::rotate(glm::mat4(1),drawList[i]->rotation.y,glm::vec3(0,1.0,0)) *
			glm::rotate(glm::mat4(1),drawList[i]->rotation.z,glm::vec3(0,0,1.0));
		glm::mat4 trans = glm::translate(glm::mat4(1), drawList[i]->position);
		glm::mat4 modelMat = rot * scale * trans;
		modelMat *= glm::rotate(glm::mat4(1),-90.0f,glm::vec3(1.0,0,0)); //Rotate everything -90deg on x axis
		glUniformMatrix4fv(prg->getUniform("modelMat"),1,GL_FALSE,glm::value_ptr(modelMat));
		drawList[i]->model->draw(prg,drawList[i]->outframe, false);
	}

	GLfloat *pixels = new GLfloat[1024*1024];
	glReadPixels(0,0,1024,1024,GL_DEPTH_COMPONENT,GL_FLOAT,pixels);

	depthimg.create(1024,1024,sf::Color::Black);
	for(unsigned int i=0,x=0,y=0;i<1024*1024;i++){
		x = i%1024;
		if(x == 0 && i>0)
		y++;
		depthimg.setPixel(x,y,sf::Color(ceil((double)(pixels[i]*255.0f)),
		ceil((double)(pixels[i]*255.0f)),
		ceil((double)(pixels[i]*255.0f)),255));
	}

	delete[] pixels;	

	glDrawBuffer(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0,0,width,height);
}

void RenderManager::render(ShaderProgram *prg, float dt){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(prg->getUniform("shadowMap"), 1);

	glm::mat4 view = glm::lookAt(glm::vec3(-4, 6, -4), glm::vec3(0,0,0), 
			glm::vec3(0, 1, 0));
	glm::mat4 proj = glm::ortho<float>(-10.0f,10.0f,-10.0f,10.0f,-10.0f,20.0f);


	glm::mat4 bias(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
	);

	glm::mat4 mv = proj*view;
	glm::mat4 depthMVP = bias*mv;
	glUniformMatrix4fv(prg->getUniform("depthMVP"), 1, GL_FALSE, glm::value_ptr(depthMVP));
	
	for(int i=0;i<this->drawList.size();i++){
		if(drawList[i]->animate){
			drawList[i]->aTime += dt; 
			drawList[i]->model->animate(drawList[i]->currentAnimation,
					drawList[i]->aTime,&drawList[i]->outframe);
		}

		glm::mat4 view = currentCam->view();
		glUniformMatrix4fv(prg->getUniform("view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 scale = glm::scale(glm::mat4(1),drawList[i]->scale);
		glm::mat4 rot = \
			glm::rotate(glm::mat4(1),drawList[i]->rotation.x,glm::vec3(1.0,0,0)) *
			glm::rotate(glm::mat4(1),drawList[i]->rotation.y,glm::vec3(0,1.0,0)) *
			glm::rotate(glm::mat4(1),drawList[i]->rotation.z,glm::vec3(0,0,1.0));
		glm::mat4 trans = glm::translate(glm::mat4(1), drawList[i]->position);
		glm::mat4 modelMat = rot * scale * trans;
		modelMat *= glm::rotate(glm::mat4(1),-90.0f,glm::vec3(1.0,0,0)); //Rotate everything -90deg on x axis
		glUniformMatrix4fv(prg->getUniform("modelMat"),1,GL_FALSE,glm::value_ptr(modelMat));
		drawList[i]->model->draw(prg,drawList[i]->outframe, true);
	}
}