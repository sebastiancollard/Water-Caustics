// CPSC 453: Introduction to Computer Graphics
//
// Header file for a class to handle obj file data
//
// Created by Troy Alderson
// Department of Computer Science
// University of Calgary
//
// Used with permission for CPSC 453

#include<glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

using namespace std;
using namespace glm;


class ObjFile {
public:
	ObjFile(char* filename);

	void calculateNormals();
	void bufferData();
	void draw(GLuint vertexLocation, GLuint normalLocation);

	int numVertices();
	vec4 getVertex(int i);
	vec3 getNormal(int i);
	void setVertex(int i, vec4 v);
	void setNormal(int i, vec3 v);

	mat4 getFitScale();
	mat4 getFitTranslate();

	GLuint vertexBuffer, indexBuffer;
	vector<GLfloat> vertices, normals;
	vector<GLuint> indices;

	vec3 max, min;
protected:

private:
	
};

