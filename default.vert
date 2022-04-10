#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 position;
// Colors
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;


// Outputs the color for the Fragment Shader
out vec3 Normal;
out vec3 FragPos;

// Imports the camera matrix from the main function
uniform mat4 camMatrix;
uniform mat4 model;
uniform float time;

//shader constants
//const float amplitude = 0.06125;
const float amplitude = 0.6125;
const float frequency = 4;
//const float amplitude = 0.2;
//const float frequency = 2;
const float PI = 3.14159;

float PHI = 1.61803398874989484820459;  // = Golden Ratio   

float gold_noise(in vec2 xy, in float seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

void main()
{
	//get the Euclidean distance of the current vertex from the center of the mesh
	//float distance = length(aPos);  
	//create a sin function using the distance, multiply frequency and add the elapsed time
	//float y = amplitude/1.5*sin(-PI*distance*frequency+time*2)*sin(-PI*aPos.x*frequency*50+time*2)*sin(-PI*aPos.x*frequency+time*3)*sin(-PI*aPos.z*frequency/2+time*4)*gold_noise(vec2(10.f), 12) + amplitude*sin(-PI*aPos.x*aPos.z*frequency/8+time*2) + amplitude*sin(-PI*aPos.x*frequency/16+time*2) + amplitude/10*sin(-PI*distance*frequency*5+time*2)*gold_noise(vec2(10.f), 100);	
	//float y= amplitude*sin(-PI*distance*frequency+time);
	//if(distance>0.5f){
	
//	float y = amplitude*sin(-PI*distance*frequency+time);
//	if(distance <0.5f){
//		y = amplitude/1.5*sin(-PI*distance*frequency+time*2)*sin(-PI*aPos.x*frequency*50+time*2)*sin(-PI*aPos.x*frequency+time*3)*sin(-PI*aPos.z*frequency/2+time*4)*gold_noise(vec2(10.f), 12) + amplitude*sin(-PI*aPos.x*aPos.z*frequency/8+time*2) + amplitude*sin(-PI*aPos.x*frequency/16+time*2) + amplitude/10*sin(-PI*distance*frequency*5+time*2)*gold_noise(vec2(10.f), 100);
//	}
//	}
//	else{/
//	y = 0.0f;
//	}
	//float y = amplitude*sin((sqrt(aPos.x * aPos.x + aPos.z * aPos.z)/1.0f + time) * -2*PI);
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(position, 1.0);
	// Assigns the colors from the Vertex Data to "color"
	Normal = normal;

	FragPos = position;
}