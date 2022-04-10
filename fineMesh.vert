#version 330 core

// Positions/Coordinates
layout (location = 0) in vec4 position;
// Colors
layout (location = 1) in vec3 normal; // This is the normals for the wave mesh


// Outputs the color for the Fragment Shader
out vec3 color;
out vec3 intercept;

// Imports the camera matrix from the main function
uniform mat4 camMatrix;
uniform mat4 model;

//uniform float samples[6561];


void main()
{
	// https://www.khronos.org/opengl/wiki/Vertex_Shader#:~:text=Vertex%20shaders%20are%20fed%20Vertex,input%20vertices%20to%20output%20vertices.
	// gl_vertexID
	gl_Position = camMatrix * position;
	// 6561
	// Get normal for location
	//float a = float(gl_VertexID) / 6561.0;
	//float x= samples[gl_VertexID*3];
	//float y= samples[gl_VertexID*3 + 1];
	//float z = samples[gl_VertexID*3 + 2];


	float distance = (0.0f - position.y)/ normal.y;
	intercept = position.xyz + (distance * normal);
	//intercept = position.xyz;
	color = normal;
}