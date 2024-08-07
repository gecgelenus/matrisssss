#pragma once

#include "Util.hpp"
#include "Object.hpp"
#include "Camera.hpp"
#include "Physics.hpp"

class Renderer {


public:
	Renderer();
	Renderer(GLuint programID);
	~Renderer();

	bool addObject(Object* object);
	void removeObject(Object* object);
	void addObjectToQueue(Object* object);
	void setPhysics(Physics* physics);
	unsigned int getObjectCount() { return objects.size(); };


	virtual void render();

	bool textureEnabled;
	bool priv;

protected:

	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
	GLuint screenHeight, screenWidth;
	glm::mat4 lightProjection, lightView;


	Physics* physics;
	
	GLuint shadowMapFBO;
	GLuint shadowMap;
	GLuint shadowShaderProgram;

	std::vector<Object*> objects;
	std::vector<Object*> objectQueue;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	std::vector<glm::mat4> modelMatrices;
	glm::vec3 lightPos;

	GLuint vertexbuffer;
	GLuint elementBuffer;
	GLuint VertexArrayID;

	GLuint modelUniformID;
	GLuint lightUniformID;
	GLuint programID;

	std::vector<bool> modelMatrixUsed;
};