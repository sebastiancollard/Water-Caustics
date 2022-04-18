
//------- Ignore this ----------
#include<filesystem>
//------------------------------

#include<iostream>
#include "objFile.h"

#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"imgui/imgui.h"
#include"imgui/imgui_impl_glfw.h"
#include"imgui/imgui_impl_opengl3.h"

#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Camera.h"
#include <vector>

using namespace std;
using namespace glm;




// Vertex array object
GLuint myVAO;
GLuint myFineMeshVAO;

// Shader program to use
GLuint waterShader;
GLuint waterMapShader;
GLuint groundShader;
GLuint causticMapShader;
GLuint causticMapBlurShader;

// Shader filenames
string waterVSFilename = "default.vert";
string waterFSFilename = "default.frag";

string groundVSFilename = "fineMesh.vert";
string groundFSFilename = "fineMesh.frag";

string causticMapVSFilename = "causticMap.vert";
string causticMapFSFilename = "causticMap.frag";

string causticMapBlurVSFilename = "causticMapBlur.vert";
string causticMapBlurFSFilename = "causticMapBlur.frag";

string waterMapVSFilename = "waterMap.vert";
string waterMapFSFilename = "waterMap.frag";

// ObjFile instance
ObjFile* waterMesh;
ObjFile* causticMesh;

ObjFile* waterMesh80;
ObjFile* causticMesh80;

void wavePresetsUpdate(ObjFile* waterMesh, float time);

// Transformation matrices
mat4 rot = mat4(1.0f);
mat4 scaling = mat4(1.0f);
mat4 translation = mat4(1.0f);
float scalar = 1.0f;

//shader constants
float amplitude = 0.06125;
float frequency = 4;
bool wavePre0 = false;
bool wavePre1 = false;
bool wavePre2 = false;
bool wavePre3 = true;
bool wavePre4 = false;

bool plane80 = false;
bool plane160 = true;
//const float amplitude = 0.2;
//const float frequency = 2;
const float PI = 3.14159;

float PHI = 1.61803398874989484820459;  // = Golden Ratio   
#define E 2.7182818

float gold_noise(vec2 xy, float seed) {
	return fract(tan(distance(xy * PHI, xy) * seed) * xy.x);
}

// Constants to help with location bindings
#define VERTEX_DATA 0
#define VERTEX_NORMAL 1

float groundOffset = 1.f;

int blurIntensity = 2;
unsigned int sampleSteps = 4;
unsigned int sunDistance = 1024;
float baseIntensity = 0.25f;

const unsigned int width = 1920;
const unsigned int height = 1080;
unsigned int gBuffer, gBufferBlur, gCaustic, gCausticBlurred;


////////////////////////////////////////////////////////////////
// Load the shader from the specified file. Returns false if the
// shader could not be loaded
static GLubyte shaderText[8192];
bool loadShaderFile(const char* filename, GLuint shader) {
	GLint shaderLength = 0;
	FILE* fp;

	// Open the shader file
	fp = fopen(filename, "r");
	if (fp != NULL) {
		// See how long the file is
		while (fgetc(fp) != EOF)
			shaderLength++;

		// Go back to beginning of file
		rewind(fp);

		// Read the whole file in
		fread(shaderText, 1, shaderLength, fp);

		// Make sure it is null terminated and close the file
		shaderText[shaderLength] = '\0';
		fclose(fp);
	}
	else {
		return false;
	}

	// Load the string into the shader object
	GLchar* fsStringPtr[1];
	fsStringPtr[0] = (GLchar*)((const char*)shaderText);
	glShaderSource(shader, 1, (const GLchar**)fsStringPtr, NULL);

	return true;
}


// Called to draw scene
void renderScene() {
	// Clear the window and the depth buffer
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(waterShader);

	translation = mat4(1.f);
	mat4 model = rot * scaling * translation;
	glUniformMatrix4fv(glGetUniformLocation(waterShader, "model"), 1, GL_FALSE, value_ptr(model));

	//Create and pass projection matrix
	mat4 proj = perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(waterShader, "proj_matrix"), 1, GL_FALSE, value_ptr(proj));

	//Set shader uniforms
	glUniform3f(glGetUniformLocation(waterShader, "light_pos"), 1.0f, 1.0f, -1.0f);


	// Note that this version of the draw command uses the
	// bound index buffer to get the vertex coordinates.
	if (plane160) {
		waterMesh->draw(VERTEX_DATA, VERTEX_NORMAL);
	}
	else if (plane80) {
		waterMesh80->draw(VERTEX_DATA, VERTEX_NORMAL);
	}
}


// This function does any needed initialization on the rendering
// context. 
void setupRenderingContext() {
	// Background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// First setup the shaders
	//Now, let's setup the shaders
	GLuint waterVertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint waterFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint waterMapVertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint waterMapFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint groundVertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint groundFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint causticMapVertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint causticMapFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint causticMapBlurVertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint causticMapBlurFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	groundShader = (GLuint)NULL;
	waterShader = (GLuint)NULL;
	waterMapShader = (GLuint)NULL;
	causticMapShader = (GLuint)NULL;
	causticMapBlurShader = (GLuint)NULL;
	GLint testVal;

	if (!loadShaderFile(waterVSFilename.c_str(), waterVertexShader)) {
		glDeleteShader(waterVertexShader);
		glDeleteShader(waterFragmentShader);
		cout << "The shader " << waterVSFilename << " could not be found." << endl;
	}

	if (!loadShaderFile(waterFSFilename.c_str(), waterFragmentShader)) {
		glDeleteShader(waterVertexShader);
		glDeleteShader(waterFragmentShader);
		cout << "The shader " << waterFSFilename << " could not be found." << endl;
	}

	if (!loadShaderFile(groundVSFilename.c_str(), groundVertexShader)) {
		glDeleteShader(groundVertexShader);
		glDeleteShader(groundFragmentShader);
		cout << "The shader " << groundVSFilename << " could not be found." << endl;
	}

	if (!loadShaderFile(groundFSFilename.c_str(), groundFragmentShader)) {
		glDeleteShader(groundVertexShader);
		glDeleteShader(groundFragmentShader);
		cout << "The shader " << groundFSFilename << " could not be found." << endl;
	}

	if (!loadShaderFile(causticMapVSFilename.c_str(), causticMapVertexShader)) {
		glDeleteShader(causticMapVertexShader);
		glDeleteShader(causticMapFragmentShader);
		cout << "The shader " << causticMapVSFilename << " could not be found." << endl;
	}

	if (!loadShaderFile(causticMapFSFilename.c_str(), causticMapFragmentShader)) {
		glDeleteShader(causticMapVertexShader);
		glDeleteShader(causticMapFragmentShader);
		cout << "The shader " << causticMapFSFilename << " could not be found." << endl;
	}

	if (!loadShaderFile(causticMapBlurVSFilename.c_str(), causticMapBlurVertexShader)) {
		glDeleteShader(causticMapBlurVertexShader);
		glDeleteShader(causticMapBlurFragmentShader);
		cout << "The shader " << causticMapBlurVSFilename << " could not be found." << endl;
	}

	if (!loadShaderFile(causticMapBlurFSFilename.c_str(), causticMapBlurFragmentShader)) {
		glDeleteShader(causticMapBlurVertexShader);
		glDeleteShader(causticMapBlurFragmentShader);
		cout << "The shader " << causticMapBlurFSFilename << " could not be found." << endl;
	}

	if (!loadShaderFile(waterMapVSFilename.c_str(), waterMapVertexShader)) {
		glDeleteShader(waterMapVertexShader);
		glDeleteShader(waterMapFragmentShader);
		cout << "The shader " << waterMapVSFilename << " could not be found." << endl;
	}


	if (!loadShaderFile(waterMapFSFilename.c_str(), waterMapFragmentShader)) {
		glDeleteShader(waterMapVertexShader);
		glDeleteShader(waterMapFragmentShader);
		cout << "The shader " << waterMapFSFilename << " could not be found." << endl;
	}

	glCompileShader(waterVertexShader);
	glCompileShader(waterFragmentShader);
	glCompileShader(groundVertexShader);
	glCompileShader(groundFragmentShader);
	glCompileShader(causticMapVertexShader);
	glCompileShader(causticMapFragmentShader);
	glCompileShader(causticMapBlurVertexShader);
	glCompileShader(causticMapBlurFragmentShader);
	glCompileShader(waterMapVertexShader);
	glCompileShader(waterMapFragmentShader);

	// Check for any error generated during shader compilation
	glGetShaderiv(waterVertexShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(waterVertexShader, 8192, NULL, source);
		glGetShaderInfoLog(waterVertexShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(waterVertexShader);
		glDeleteShader(waterFragmentShader);
	}
	glGetShaderiv(waterFragmentShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(waterFragmentShader, 8192, NULL, source);
		glGetShaderInfoLog(waterFragmentShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(waterVertexShader);
		glDeleteShader(waterFragmentShader);
	}

	glGetShaderiv(groundVertexShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(groundVertexShader, 8192, NULL, source);
		glGetShaderInfoLog(groundVertexShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(groundVertexShader);
		glDeleteShader(groundFragmentShader);
	}
	glGetShaderiv(groundFragmentShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(groundFragmentShader, 8192, NULL, source);
		glGetShaderInfoLog(groundFragmentShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(groundVertexShader);
		glDeleteShader(groundFragmentShader);
	}

	glGetShaderiv(causticMapVertexShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(causticMapVertexShader, 8192, NULL, source);
		glGetShaderInfoLog(causticMapVertexShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(causticMapVertexShader);
		glDeleteShader(causticMapFragmentShader);
	}
	glGetShaderiv(causticMapFragmentShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(causticMapFragmentShader, 8192, NULL, source);
		glGetShaderInfoLog(causticMapFragmentShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(causticMapVertexShader);
		glDeleteShader(causticMapFragmentShader);
	}

	glGetShaderiv(causticMapBlurVertexShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(causticMapBlurVertexShader, 8192, NULL, source);
		glGetShaderInfoLog(causticMapBlurVertexShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(causticMapBlurVertexShader);
		glDeleteShader(causticMapBlurFragmentShader);
	}
	glGetShaderiv(causticMapBlurFragmentShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(causticMapBlurFragmentShader, 8192, NULL, source);
		glGetShaderInfoLog(causticMapBlurFragmentShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(causticMapBlurVertexShader);
		glDeleteShader(causticMapBlurFragmentShader);
	}

	glGetShaderiv(waterMapVertexShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(waterMapVertexShader, 8192, NULL, source);
		glGetShaderInfoLog(waterMapVertexShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(waterMapVertexShader);
		glDeleteShader(waterMapFragmentShader);
	}
	glGetShaderiv(waterMapFragmentShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(waterMapFragmentShader, 8192, NULL, source);
		glGetShaderInfoLog(waterMapFragmentShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(waterMapVertexShader);
		glDeleteShader(waterMapFragmentShader);
	}

	// Create the shader program and bind locations for the vertex
	// attributes before linking. The linking process can also generate errors

	waterShader = glCreateProgram();
	glAttachShader(waterShader, waterVertexShader);
	glAttachShader(waterShader, waterFragmentShader);

	glBindAttribLocation(waterShader, VERTEX_DATA, "position");
	//glBindAttribLocation( myShaderProgram, VERTEX_COLOUR, "vColor" );
	glBindAttribLocation(waterShader, VERTEX_NORMAL, "normal");
	glBindAttribLocation(waterShader, 2, "texcoord");

	glLinkProgram(waterShader);
	glDeleteShader(waterVertexShader);
	glDeleteShader(waterFragmentShader);
	glGetProgramiv(waterShader, GL_LINK_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char infoLog[1024];
		glGetProgramInfoLog(waterShader, 1024, NULL, infoLog);
		cout << "The shader program (" << waterVSFilename << " + " << waterFSFilename << ") failed to link:" << endl << (const char*)infoLog << endl;
		glDeleteProgram(waterShader);
		waterShader = (GLuint)NULL;
	}

	waterMapShader = glCreateProgram();
	glAttachShader(waterMapShader, waterMapVertexShader);
	glAttachShader(waterMapShader, waterMapFragmentShader);

	glBindAttribLocation(waterMapShader, VERTEX_DATA, "position");
	//glBindAttribLocation( myShaderProgram, VERTEX_COLOUR, "vColor" );
	glBindAttribLocation(waterMapShader, VERTEX_NORMAL, "normal");
	glBindAttribLocation(waterMapShader, 2, "texcoord");

	glLinkProgram(waterMapShader);
	glDeleteShader(waterMapVertexShader);
	glDeleteShader(waterMapFragmentShader);
	glGetProgramiv(waterMapShader, GL_LINK_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char infoLog[1024];
		glGetProgramInfoLog(waterMapShader, 1024, NULL, infoLog);
		cout << "The shader program (" << waterMapVSFilename << " + " << waterMapFSFilename << ") failed to link:" << endl << (const char*)infoLog << endl;
		glDeleteProgram(waterMapShader);
		waterMapShader = (GLuint)NULL;
	}

	groundShader = glCreateProgram();
	glAttachShader(groundShader, groundVertexShader);
	glAttachShader(groundShader, groundFragmentShader);

	glBindAttribLocation(groundShader, VERTEX_DATA, "position");
	//glBindAttribLocation( myShaderProgram, VERTEX_COLOUR, "vColor" );
	glBindAttribLocation(groundShader, VERTEX_NORMAL, "normal");
	glBindAttribLocation(groundShader, 2, "texcoord");

	glLinkProgram(groundShader);
	glDeleteShader(groundVertexShader);
	glDeleteShader(groundFragmentShader);
	glGetProgramiv(groundShader, GL_LINK_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char infoLog[1024];
		glGetProgramInfoLog(groundShader, 1024, NULL, infoLog);
		cout << "The shader program (" << groundVSFilename << " + " << groundFSFilename << ") failed to link:" << endl << (const char*)infoLog << endl;
		glDeleteProgram(groundShader);
		groundShader = (GLuint)NULL;
	}

	causticMapShader = glCreateProgram();
	glAttachShader(causticMapShader, causticMapVertexShader);
	glAttachShader(causticMapShader, causticMapFragmentShader);

	glBindAttribLocation(causticMapShader, VERTEX_DATA, "position");
	//glBindAttribLocation( myShaderProgram, VERTEX_COLOUR, "vColor" );
	glBindAttribLocation(causticMapShader, VERTEX_NORMAL, "normal");
	glBindAttribLocation(causticMapShader, 2, "texcoord");

	glLinkProgram(causticMapShader);
	glDeleteShader(causticMapVertexShader);
	glDeleteShader(causticMapFragmentShader);
	glGetProgramiv(causticMapShader, GL_LINK_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char infoLog[1024];
		glGetProgramInfoLog(causticMapShader, 1024, NULL, infoLog);
		cout << "The shader program (" << causticMapVSFilename << " + " << causticMapFSFilename << ") failed to link:" << endl << (const char*)infoLog << endl;
		glDeleteProgram(causticMapShader);
		causticMapShader = (GLuint)NULL;
	}

	causticMapBlurShader = glCreateProgram();
	glAttachShader(causticMapBlurShader, causticMapBlurVertexShader);
	glAttachShader(causticMapBlurShader, causticMapBlurFragmentShader);

	glBindAttribLocation(causticMapBlurShader, VERTEX_DATA, "position");
	//glBindAttribLocation( myShaderProgram, VERTEX_COLOUR, "vColor" );
	glBindAttribLocation(causticMapBlurShader, VERTEX_NORMAL, "normal");

	glLinkProgram(causticMapBlurShader);
	glDeleteShader(causticMapBlurVertexShader);
	glDeleteShader(causticMapBlurFragmentShader);
	glGetProgramiv(causticMapBlurShader, GL_LINK_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char infoLog[1024];
		glGetProgramInfoLog(causticMapBlurShader, 1024, NULL, infoLog);
		cout << "The shader program (" << causticMapBlurVSFilename << " + " << causticMapBlurFSFilename << ") failed to link:" << endl << (const char*)infoLog << endl;
		glDeleteProgram(causticMapBlurShader);
		causticMapBlurShader = (GLuint)NULL;
	}

	// Now setup the geometry in a vertex buffer object

	// setup the vertex state array object. All subsequent buffers will
	// be bound to it.
	glGenVertexArrays(1, &myVAO);
	glBindVertexArray(myVAO);

	// configure g-buffer framebuffer
	// ------------------------------
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glGenTextures(1, &gCaustic);
	glBindTexture(GL_TEXTURE_2D, gCaustic);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 3840, 3840, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gCaustic, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &gBufferBlur);
	glBindFramebuffer(GL_FRAMEBUFFER, gBufferBlur);
	glGenTextures(1, &gCausticBlurred);
	glBindTexture(GL_TEXTURE_2D, gCausticBlurred);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 3840, 3840, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gCausticBlurred, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "caustic Blur Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


int main(int argc, char** argv)
{
	//												//
	// TEST I believe there are some LINKER ISSUES //
	//						

	if (argc != 2) {
		cout << "Usage: " << argv[0] << " filename\n";
		exit(EXIT_FAILURE);
	}

	// Initalization of Planes
	char plane[] = "./models/plane.obj";
	char plane2[] = "./models/plane2.obj";

	waterMesh = new ObjFile(plane2);
	if (waterMesh->numVertices() == 0) {
		cout << "Could not load file " << argv[1] << ".\n";
		exit(EXIT_FAILURE);
	}
	causticMesh = new ObjFile(plane2);

	waterMesh80 = new ObjFile(plane);
	if (waterMesh80->numVertices() == 0) {
		cout << "Could not load file " << argv[1] << ".\n";
		exit(EXIT_FAILURE);
	}
	causticMesh80 = new ObjFile(plane);
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "YoutubeOpenGL", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	//std::cout << "HERE";
	glfwMakeContextCurrent(window);
	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);

	/*
	namespace givio = giv::io; // perhaps better than giv::io
	givio::GLFWContext glContext;
	glContext.glMajorVesion(4)
		.glMinorVesion(0)
		.glForwardComaptability(true)
		.glCoreProfile()
		.glAntiAliasingSamples(4)
		.matchPrimaryMonitorVideoMode();

	std::cout << givio::glfwVersionString() << '\n';
	*/


	//Texture environmentMap("textures/EnvironmentMap.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	//environmentMap.texUnit(fineMeshShader, "tex0", 0);

	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	int widthI, heightI, nrChannels;
	//stbi_set
	//stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load("./textures/env.png", &widthI, &heightI, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthI, heightI, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



	data = stbi_load("./textures/vlziefgfw_2K_Albedo.jpg", &widthI, &heightI, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthI, heightI, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);






	//// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
	//// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	setupRenderingContext();
	waterMesh->calculateNormals();
	glGenBuffers(1, &waterMesh->vertexBuffer);
	glGenBuffers(1, &waterMesh->indexBuffer);
	waterMesh->bufferData(waterMesh->normals);

	causticMesh->calculateNormals();
	glGenBuffers(1, &causticMesh->vertexBuffer);
	glGenBuffers(1, &causticMesh->indexBuffer);
	causticMesh->bufferData(causticMesh->normals);

	waterMesh80->calculateNormals();
	glGenBuffers(1, &waterMesh80->vertexBuffer);
	glGenBuffers(1, &waterMesh80->indexBuffer);
	waterMesh80->bufferData(waterMesh80->normals);

	causticMesh80->calculateNormals();
	glGenBuffers(1, &causticMesh80->vertexBuffer);
	glGenBuffers(1, &causticMesh80->indexBuffer);
	causticMesh80->bufferData(causticMesh80->normals);


	for (int i = 0; i < causticMesh->vertices.size(); i += 4) {
		//causticMesh->vertices[i + 1] = -groundOffset;


		float x = causticMesh->vertices[i];
		float z = causticMesh->vertices[i + 2];
		causticMesh->tex.push_back((x + 2.0f) / 4.0f);
		causticMesh->tex.push_back((z + 2.0f) / 4.0f);

		waterMesh->tex.push_back((z + 2.0f) / 4.0f);
		waterMesh->tex.push_back((z + 2.0f) / 4.0f);
		// vec2( (pos.x/4)+2, (pos.z/4)+2 )
	}

	causticMesh->bufferData(waterMesh->normals);

	for (int i = 0; i < causticMesh80->vertices.size(); i += 4) {
		//causticMesh80->vertices[i + 1] = -groundOffset;


		float x = causticMesh80->vertices[i];
		float z = causticMesh80->vertices[i + 2];
		causticMesh80->tex.push_back((x + 2.0f) / 4.0f);
		causticMesh80->tex.push_back((z + 2.0f) / 4.0f);

		waterMesh80->tex.push_back((z + 2.0f) / 4.0f);
		waterMesh80->tex.push_back((z + 2.0f) / 4.0f);
		// vec2( (pos.x/4)+2, (pos.z/4)+2 )
	}

	causticMesh80->bufferData(waterMesh80->normals);



	std::cout << "MAXSIZE: " << GL_MAX_TEXTURE_SIZE << "\n";

	unsigned int gBufferWater;
	glGenFramebuffers(1, &gBufferWater);
	glBindFramebuffer(GL_FRAMEBUFFER, gBufferWater);
	unsigned int gPosition, gNormal;

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// variables for FPS counter
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;




	// Main while loop
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");




	while (!glfwWindowShouldClose(window))
	{



		// Specify the color of the background
		glClearColor(51.f / 255.f, 71.f / 255.f, 79.f / 255.f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();



		// update variables
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;
		if (timeDiff >= 1.0 / 30.0)
		{
			std::string FPS = std::to_string((int)((1.0 / timeDiff) * counter));
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			std::string newTitle = "CPSC 591 Water Caustics - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());

			// reset timer/counter
			prevTime = crntTime;
			counter = 0;
		}

		causticMesh->bufferData(waterMesh->normals);
		causticMesh80->bufferData(waterMesh80->normals);


		// disable blending for framebuffer stuff
		glDisable(GL_BLEND);

		// FrameBuffer geomtry of water mesh:

		glBindFramebuffer(GL_FRAMEBUFFER, gBufferWater);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(waterMapShader);
		glBindVertexArray(myVAO);

		scaling = scale(mat4(1.0f), vec3(0.5));
		rot = rotate(mat4(1.f), glm::radians(90.f), glm::vec3(1, 0, 0));
		translation = mat4(1.f);
		//	translation = translate(mat4(1.f), glm::vec3(0, -groundOffset, 0));
		mat4 model = rot * scaling * translation;
		glUniformMatrix4fv(glGetUniformLocation(waterMapShader, "model"), 1, GL_FALSE, value_ptr(model));

		if (plane160) {
			waterMesh->draw(VERTEX_DATA, VERTEX_NORMAL);
		}
		else if (plane80) {
			waterMesh80->draw(VERTEX_DATA, VERTEX_NORMAL);
		}
		//causticMesh->draw(VERTEX_DATA, VERTEX_NORMAL);

		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		//waterMesh->draw(VERTEX_DATA, VERTEX_NORMAL);

		glViewport(0, 0, 3840, 3840);
		// first framebuffer: caustic map
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(causticMapShader);
		glBindVertexArray(myVAO);

		glUniform1i(glGetUniformLocation(causticMapShader, "texture1"), 0);
		glUniform1i(glGetUniformLocation(causticMapShader, "gNormal"), 1);

		glUniform1f(glGetUniformLocation(causticMapShader, "groundOffset"), groundOffset);

		glUniform1i(glGetUniformLocation(causticMapShader, "sampleSteps"), sampleSteps);
		glUniform1i(glGetUniformLocation(causticMapShader, "sunDistance"), sunDistance);
		glUniform1f(glGetUniformLocation(causticMapShader, "baseIntensity"), baseIntensity);

		model = rot * scaling * translation;
		glUniformMatrix4fv(glGetUniformLocation(causticMapShader, "model"), 1, GL_FALSE, value_ptr(model));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);

		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_1D, texture);

		if (plane160) {
			causticMesh->draw(VERTEX_DATA, VERTEX_NORMAL);
		}
		else if (plane80) {
			causticMesh80->draw(VERTEX_DATA, VERTEX_NORMAL);
		}

		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// second framebuffer: caustic map blurred
		glBindFramebuffer(GL_FRAMEBUFFER, gBufferBlur);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(causticMapBlurShader);
		glBindVertexArray(myVAO);

		glUniform1i(glGetUniformLocation(causticMapBlurShader, "gCaustic"), 0);
		glUniform1i(glGetUniformLocation(causticMapBlurShader, "blurIntensity"), blurIntensity);
		model = rot * scaling * translation;
		glUniformMatrix4fv(glGetUniformLocation(causticMapBlurShader, "model"), 1, GL_FALSE, value_ptr(model));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gCaustic);

		if (plane160) {
			causticMesh->draw(VERTEX_DATA, VERTEX_NORMAL);
		}
		else if (plane80) {
			causticMesh80->draw(VERTEX_DATA, VERTEX_NORMAL);
		}

		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);






		// now we can enable blending 
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);


		//glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(groundShader);
		glBindVertexArray(myVAO);

		// env map, sand texture and caustic texture
		glUniform1i(glGetUniformLocation(groundShader, "texture1"), 0);
		glUniform1i(glGetUniformLocation(groundShader, "texture2"), 1);
		glUniform1i(glGetUniformLocation(groundShader, "gCausticBlurred"), 2);
		glUniform1i(glGetUniformLocation(groundShader, "gp"), 3);
		glUniform1f(glGetUniformLocation(causticMapShader, "groundOffset"), groundOffset);

		//glUniform1i(glGetUniformLocation(groundShader, "gp"), 0);
		//glUniform1i(glGetUniformLocation(groundShader, "gpp"), 1);
		//glUniform1i(glGetUniformLocation(groundShader, "texture1"), 2);
		//glUniform1i(glGetUniformLocation(groundShader, "texture2"), 3);
		//glUniform1i(glGetUniformLocation(groundShader, "gCausticBlurred"), 4);


		if (!io.WantCaptureMouse) {
			camera.Inputs(window);
		}
		// Updates and exports the camera matrix to the Vertex Shader
		camera.Matrix(45.0f, 0.1f, 100.0f, groundShader, "camMatrix");

		//Scale based on input
		scaling = mat4(1.f);
		rot = mat4(1.f);
		translation = translate(mat4(1.f), glm::vec3(0, -groundOffset, 0));

		//Create and pass model view matrix
		mat4 modelView = lookAt(vec3(0.0f, 0.0f, -10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		modelView = rot * scaling * translation;
		model = rot * scaling * translation;
		glUniformMatrix4fv(glGetUniformLocation(groundShader, "model"), 1, GL_FALSE, value_ptr(model));

		//Create and pass projection matrix
		mat4 proj = perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(groundShader, "proj_matrix"), 1, GL_FALSE, value_ptr(proj));


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gCausticBlurred);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gNormal);

		if (plane160) {
			causticMesh->draw(VERTEX_DATA, VERTEX_NORMAL);
		}
		else if (plane80) {
			causticMesh80->draw(VERTEX_DATA, VERTEX_NORMAL);
		}

		glBindVertexArray(0);




		// DRAW WAVE LAST, FOR BLENDING/TRANSPARENCY

		// Tell OpenGL which Shader Program we want to use
		//shaderProgram.Activate();
		//std::cout << glfwGetTime() << "\n";
		glUseProgram(waterShader);
		//glUniform1f(glGetUniformLocation(shaderProgram.ID, "time"), glfwGetTime());
		float time = glfwGetTime();


		
			int vertSize = 0;
			if (plane160) {
				wavePresetsUpdate(waterMesh, time);
			}
			else if (plane80) {
				wavePresetsUpdate(waterMesh80, time);
			}




			// Handles camera inputs

			if (!io.WantCaptureMouse) {
				camera.Inputs(window);
			}
			// Updates and exports the camera matrix to the Vertex Shader
			camera.Matrix(45.0f, 0.1f, 100.0f, waterShader, "camMatrix");
			glUniform3fv(glGetUniformLocation(waterShader, "viewPos"), 1, &camera.Position[0]);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glBindVertexArray(myVAO);
			renderScene();
			glBindVertexArray(0);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			ImGui::Begin("SandBox!");

			if (ImGui::Button("80 x 80 Plane")) {

				plane80 = true;
				plane160 = false;

			}
			ImGui::SameLine();
			if (ImGui::Button("160 x 160 Plane")) {
				plane80 = false;
				plane160 = true;
			}
			if (ImGui::Button("Wave Preset 0")) {

				wavePre0 = true;

				wavePre1 = false;
				wavePre2 = false;
				wavePre3 = false;
				wavePre4 = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Wave Preset 1")) {

				wavePre1 = true;

				wavePre0 = false;
				wavePre2 = false;
				wavePre3 = false;
				wavePre4 = false;
			}
			if (ImGui::Button("Wave Preset 2")) {

				wavePre2 = true;

				wavePre0 = false;
				wavePre1 = false;
				wavePre3 = false;
				wavePre4 = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Wave Preset 3")) {

				wavePre3 = true;

				wavePre0 = false;
				wavePre1 = false;
				wavePre2 = false;
				wavePre4 = false;
			}

			if (ImGui::Button("Wave Preset 4")) {

				wavePre4 = true;

				wavePre0 = false;
				wavePre1 = false;
				wavePre2 = false;
				wavePre3 = false;
			}
			ImGui::SliderFloat("Wave Amplitude", &amplitude, 0.01f, 0.4f);
			ImGui::SliderFloat("Frequency", &frequency, 0.f, 8.f);
			ImGui::SliderFloat("Ground Offset", &groundOffset, 0.4f, 5.f);
			ImGui::SliderFloat("Base Intensity", &baseIntensity, 0.f, 1.f);
			ImGui::SliderInt("Blur Intensity", &blurIntensity, 0, 5);
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
			// Take care of all GLFW events
			glfwPollEvents();

			//std::cout << "Normals: " << file->normals.size()<< "\n";
			//std::cout << "Vertices: " << file->vertices.size() << "\n";

			//std::cout << "Normals1: " << fineFile->normals.size() << "\n";
			//std::cout << "Vertices1: " << fineFile->vertices.size() << "\n";
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		// Delete all the objects we've created
		//brickTex.Delete();
		//shaderProgram.Delete();
		// Delete window before ending the program
		glfwDestroyWindow(window);
		// Terminate GLFW before ending the program
		glfwTerminate();
		return 0;
	
}

void wavePresetsUpdate(ObjFile *waterMesh, float time) {
	int vertSize = waterMesh->vertices.size();
	for (int i = 0; i < vertSize; i += 4) {
		float x = waterMesh->vertices[i];
		float z = waterMesh->vertices[i + 2];
		float dist = glm::length(glm::vec3(x, 0, z));
		//waterMesh->vertices[i + 1] = 0;
		GLfloat temp = 0.f;
		float time2 = time * 2;
		float timemil = time + 30000;
		// plane.obj
		if (wavePre0) {
			temp = amplitude / 1.5 * sin(-PI * dist * frequency + time * 2) * ((sin(-PI * x * frequency * i + time2) + 1) / 2) * sin(-PI * x * frequency + time * 3) * sin(-PI * z * frequency / 2 + time * 4);
			temp += amplitude * sin(-PI * x * z * frequency / 8 + time2);
			temp += amplitude * sin(-PI * x * frequency / 16 + time2);
			temp += amplitude / 10 * sin(-PI * dist * frequency * 5 + time2);
			waterMesh->vertices[i + 1] = temp;
		}

		// plane2.obj
		// BASE
		else if (wavePre1) {
			temp = amplitude / 10.f * (
				0.2 * (
					-3.2 * sin(-1.3 * (1 - dist) * timemil * 4)
					- 1.2 * sin(-1.7 * E * z * timemil)
					+ 1.9 * sin(1.6 * PI * x * timemil)
					)
				);
			temp += amplitude * sin(-PI * x * frequency / 16 + time2);
			temp += amplitude * sin(-PI * x * z * frequency / 8 + time2);
			temp += amplitude / 50.f * sin(-PI * dist * frequency * 5 + time2);
			temp += amplitude / 1.5 * sin(-PI * dist * frequency + time * 2) * ((sin(-PI * x * frequency * i + time2) + 1) / 2) * sin(-PI * x * frequency + time * 3) * sin(-PI * z * frequency / 2 + time * 4);
			waterMesh->vertices[i + 1] = temp;


		}

		// PATTERN 1
		//
		else if (wavePre2) {
			blurIntensity = 1;
			sampleSteps = 5;
			sunDistance = 32;
			baseIntensity = 0.25f;
			temp += amplitude * sin(-PI * x * frequency / 16 + time2);
			temp += amplitude * sin(-PI * x * z * frequency / 8 + time2);
			temp += amplitude / 50.f * sin(-PI * dist * frequency * 5 + time2);
			temp += amplitude / 1.5 * sin(-PI * dist * frequency + time * 2) * sin(-PI * x * frequency + time * 3) * sin(-PI * z * frequency / 2 + time * 4);
			waterMesh->vertices[i + 1] = temp;


		}

		// PATTERN 2
		else if (wavePre3) {
			//blurIntensity = 2;
			sampleSteps = 4;
			sunDistance = 128;
			//baseIntensity = 0.2f;
			temp = amplitude / 10.f * (
				0.2 * (
					-3.2 * sin(-1.3 * PHI * (1 - dist / 2) * timemil * 4)
					- 1.2 * sin(-1.7 * E * z * time)
					+ 1.9 * sin(1.7 * PI * x * timemil)
					)
				);
			temp += amplitude / 50.f * sin(-PI * dist * frequency * 5 + time2);
			waterMesh->vertices[i + 1] = temp;


		}

		// PATTERN 3
		else if (wavePre4) {
			temp = amplitude / 10.f * (
				0.2 * (
					-3.2 * sin(-1.3 * (1 - dist) * timemil * 4)
					- 1.2 * sin(-1.7 * E * z * timemil)
					+ 1.9 * sin(1.6 * PI * x * timemil)
					)
				);
			waterMesh->vertices[i + 1] = temp;


		}
	}
	waterMesh->calculateNormals();
	waterMesh->bufferData(waterMesh->normals);
}






// REFERENCES:
/*
https://learnopengl.com/Getting-started/Textures 
https://subscription.packtpub.com/book/game-development/9781849695046/1/ch01lvl1sec12/doing-a-ripple-mesh-deformer-using-the-vertex-shader
https://www.gamasutra.com/view/feature/2811/inexpensive_underwater_caustics_.php?print=1
https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping 
https://stackoverflow.com/questions/5192068/c-char-argv-vs-char-argv
https://www.youtube.com/watch?v=VRwhNKoxUtk&ab_channel=VictorGordan
*/