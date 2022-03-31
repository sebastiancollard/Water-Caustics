// CPSC 453: Introduction to Computer Graphics
//
// Implementation file for a class to handle obj file data
//
// Created by Troy Alderson
// Department of Computer Science
// University of Calgary
//
// Used with permission for CPSC 453

#include "objFile.h"
#include <fstream>
#include <string>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
using namespace glm;


ObjFile::ObjFile(char* filename) {
	ifstream reader(filename);

	if (reader.is_open()) {
		char c[256];
		string str;
		GLfloat f;
		GLuint i;

		while (!reader.eof()) {
			reader >> str;

			if (str == "v") {
				reader >> f;
				vertices.push_back(f);
				if (f > max.x) max.x = f;
				if (f < min.x) min.x = f;

				reader >> f;
				vertices.push_back(f);
				if (f > max.y) max.y = f;
				if (f < min.y) min.y = f;

				reader >> f;
				vertices.push_back(f);
				if (f > max.z) max.z = f;
				if (f < min.z) min.z = f;

				vertices.push_back(1.0f);
			}
			if (str == "f") {
				reader >> i;
				indices.push_back(i - 1);
				reader >> i;
				indices.push_back(i - 1);
				reader >> i;
				indices.push_back(i - 1);
			}

			str = "";
		}

		reader.close();
	}
}


void ObjFile::draw(GLuint vertexLocation, GLuint normalLocation) {
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	glEnableVertexAttribArray(vertexLocation);
	glVertexAttribPointer(vertexLocation, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(normalLocation);
	glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(sizeof(GLfloat) * vertices.size()));

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

int ObjFile::numVertices() {
	return vertices.size() / 4;
}

vec4 ObjFile::getVertex(int i) {
	return vec4(vertices[4 * i], vertices[4 * i + 1], vertices[4 * i + 2], vertices[4 * i + 3]);
}

vec3 ObjFile::getNormal(int i) {
	return vec3(normals[3 * i], normals[3 * i + 1], normals[3 * i + 2]);
}

void ObjFile::setVertex(int i, vec4 v) {
	vertices[4 * i] = v.x;
	vertices[4 * i + 1] = v.y;
	vertices[4 * i + 2] = v.z;
	vertices[4 * i + 3] = v.w;
}

void ObjFile::setNormal(int i, vec3 v) {
	normals[3 * i] = v.x;
	normals[3 * i + 1] = v.y;
	normals[3 * i + 2] = v.z;
}

mat4 ObjFile::getFitScale() {
	float radius = length(max - min) * 0.5f;
	return scale(mat4(1.0f), vec3(0.95f / radius));
}

mat4 ObjFile::getFitTranslate() {
	vec3 center = (max + min) * 0.5f;
	return translate(mat4(1.0f), -center);
}

void ObjFile::calculateNormals() {
	normals.clear();
	for (int i = 0; i < vertices.size(); i += 4) {
		normals.push_back(0.0f);
		normals.push_back(0.0f);
		normals.push_back(0.0f);
	}

	for (int i = 0; i < indices.size(); i += 3) {
		int i1 = indices[i];
		int i2 = indices[i + 1];
		int i3 = indices[i + 2];

		vec4 p1 = getVertex(i1);
		vec4 p2 = getVertex(i2);
		vec4 p3 = getVertex(i3);
		vec3 fNormal = cross(vec3(p2 - p1), vec3(p3 - p1));

		float l = length(fNormal);
		//if (fNormal.x != 0.0f && fNormal.y != 0.0f && fNormal.z != 0.0f) {
			fNormal = normalize(fNormal);
			setNormal(i1, getNormal(i1) + fNormal);
			setNormal(i2, getNormal(i2) + fNormal);
			setNormal(i3, getNormal(i3) + fNormal);
		//}
	}

	for (int i = 0; i < normals.size(); i += 3) {
		setNormal(i / 3, normalize(getNormal(i / 3)));
	}
}

void ObjFile::bufferData() {
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);


	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * vertices.size() + sizeof(GLfloat) * normals.size(),
		NULL,
		GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER,
		0,
		sizeof(GLfloat) * vertices.size(),
		vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * vertices.size(),
		sizeof(GLfloat) * normals.size(),
		normals.data());

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(GLuint) * indices.size(),
		indices.data(),
		GL_STATIC_DRAW);
}

