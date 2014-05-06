#include <iostream>
#include <algorithm>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>
#include "Networking/server.hpp"
#include "GameObject.hpp"
#include "globals.hpp"
using namespace std;

GameObject::GameObject():
position(0,0,0), rotation(0,0,0), scale(1,1,1){

	magic = GOMAGIC;
	id = 0;
	tag = "None";
	moved = false;
	scaled = false;
	rotated = false;
	visible = true;
	animate = false;
	hasAnimation = false;
	aTime = 0;
	mass = 1;

	lookat = glm::vec3(1, 0, 0);
	right = glm::vec3(0, 0 ,1);

	rot = btQuaternion(0,0,0,1);

	body = NULL;
	trimesh = NULL;
	collisionshape = NULL;
	motion = NULL;
}

GameObject::~GameObject(){
	if(body != NULL){
		physworld.removeBody(body);
		delete body;
	}
	if(trimesh != NULL)
		delete trimesh;
	if(collisionshape != NULL)
		delete collisionshape;
	if(motion != NULL)
		delete motion;
}

void GameObject::setModel(Model *model){
	this->model = model;
	outframe.resize(model->joints.size());
	if(model->animNames.size() > 0){
		currentAnimation = model->animNames[0];
		hasAnimation = true;
	}
	textures = model->textureIDS;
	extents extents = getExtents();
}

void GameObject::move(float amount){
	position += amount*lookat;
}
void GameObject::strafe(float amount){
	position += amount*glm::normalize(right);
}
void GameObject::turn(glm::vec2 amount){
	rotation += glm::vec3(amount.y, -amount.x, 0);
	updateLookat();
}
void GameObject::updateLookat(){
	rotation.x += 90.0;
	lookat.x = sin(toRad(rotation.x)) * cos(toRad(rotation.y));
	lookat.y = cos(toRad(rotation.x));
	lookat.z = sin(toRad(rotation.x)) * sin(toRad(rotation.y));
	right = glm::cross(lookat,glm::vec3(0,1,0));
	rotation.x -= 90.0;
}
void GameObject::createTriangleRigidBody(){
	if(trimesh != NULL)
		delete trimesh;
	trimesh = new btTriangleMesh();
	//we need to rotate the triangles -90 on the x axis to fix the orientation
	glm::mat4 rot = glm::rotate(glm::mat4(1),-90.0f,glm::vec3(1,0,0));
	for(int i=0;i<model->triangles.size();i++){
		int p1 = model->triangles[i].vertex[0];
		int p2 = model->triangles[i].vertex[1];
		int p3 = model->triangles[i].vertex[2];
		glm::vec4 v1(model->verts[p1].position[0],
				model->verts[p1].position[1],
				model->verts[p1].position[2],1);
		glm::vec4 v2(model->verts[p2].position[0],
				model->verts[p2].position[1],
				model->verts[p2].position[2],1);
		glm::vec4 v3(model->verts[p3].position[0],
				model->verts[p3].position[1],
				model->verts[p3].position[2],1);

		v1 = rot*v1;
		v2 = rot*v2;
		v3 = rot*v3;

		btVector3 bV1(v1.x,v1.y,v1.z);
		btVector3 bV2(v2.x,v2.y,v2.z);
		btVector3 bV3(v3.x,v3.y,v3.z);

		trimesh->addTriangle(bV1,bV2,bV3);

	}

	if(collisionshape != NULL)
		delete collisionshape;
	collisionshape = new btBvhTriangleMeshShape(trimesh,true);
	if(motion != NULL)
		delete motion;
	motion = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),
			btVector3(0,0,0)));

	if(body != NULL){
		physworld.removeBody(body);
		delete body;
	}
	btVector3 inertia;
	collisionshape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo ci(mass,motion,collisionshape,inertia);
	body = new btRigidBody(ci);

	physworld.addBody(body);
}
void GameObject::createConvexRigidBody(){
	btConvexHullShape  *o = new btConvexHullShape();

	for(int i=0;i<model->verts.size();i++){
		o->addPoint(btVector3(model->verts[i].position[0],
					model->verts[i].position[1],
					model->verts[i].position[2]),true);
	}
	o->recalcLocalAabb();

	btShapeHull *hull = new btShapeHull(o);
	btScalar margin = o->getMargin();
	hull->buildHull(margin);
	o->setUserPointer(hull);

	if(collisionshape != NULL)
		delete collisionshape;
	collisionshape = new btConvexHullShape();
	btConvexHullShape *tmp = (btConvexHullShape *)collisionshape;
	for(int i=0;i<hull->numVertices();i++){
		tmp->addPoint(hull->getVertexPointer()[i],true);
	}
	tmp->recalcLocalAabb();
	if(motion != NULL)
		delete motion;
	motion = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),
			btVector3(0,0,0)));

	if(body != NULL){
		physworld.removeBody(body);
		delete body;
	}
	btVector3 inertia;
	collisionshape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo ci(mass,motion,collisionshape,inertia);
	body = new btRigidBody(ci);

	physworld.addBody(body);
	delete hull;
	delete o;
}
void GameObject::createCubeRigidBody(){
	extents extents = getExtents();
	float xsize,ysize,zsize;
	xsize = extents.maxx-extents.minx;
	ysize = extents.maxy-extents.miny;
	zsize = extents.maxz-extents.minz;
	btVector3 boxVector(xsize/2.0,ysize/2.0,zsize/2.0);

	if(collisionshape != NULL)
		delete collisionshape;
	collisionshape = new btBoxShape(boxVector);

	if(motion != NULL)
		delete motion;
	motion = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));

	btVector3 inertia;
	collisionshape->calculateLocalInertia(mass,inertia);
	btRigidBody::btRigidBodyConstructionInfo ci(mass,motion,collisionshape,inertia);
	if(body != NULL){
		physworld.removeBody(body);
		delete body;
	}
	body = new btRigidBody(ci);
	physworld.addBody(body);
}

void GameObject::updateMass(float mass){
	this->mass = btScalar(mass);
	btVector3 inertia;
	if(body != NULL){
		physworld.removeBody(body);
		body->getCollisionShape()->calculateLocalInertia(this->mass, inertia);
		body->setMassProps(this->mass, inertia);
		physworld.addBody(body);
	}
	else
		cout << "body is null" << endl;
}

extents GameObject::getExtents(){
	extents out;
	out.minx = model->verts[0].position[0];
	out.miny = model->verts[0].position[1];
	out.minz = model->verts[0].position[2];
	out.maxx = out.minx;
	out.maxy = out.miny;
	out.maxz = out.minz;
	for(int i=0;i<model->verts.size();i++){
		out.minx = min(out.minx, model->verts[i].position[0]);
		out.maxx = max(out.maxx, model->verts[i].position[0]);
		out.miny = min(out.miny, model->verts[i].position[1]);
		out.maxy = max(out.maxy, model->verts[i].position[1]);
		out.minz = min(out.minz, model->verts[i].position[2]);
		out.maxz = max(out.maxz, model->verts[i].position[2]);
	}
	return out;
}
