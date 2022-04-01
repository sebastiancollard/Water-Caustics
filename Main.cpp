
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

#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Camera.h"
#include <vector>

using namespace std;
using namespace glm;

void renderQuad();

// Vertex array object
GLuint myVAO;
GLuint myFineMeshVAO;

// Shader program to use
GLuint myShaderProgram;
GLuint fineMeshShader;

// Shader filenames
string vsFilename = "default.vert";
string fsFilename = "default.frag";

string vsFineFilename = "fineMesh.vert";
string fsFineFilename = "fineMesh.frag";

// ObjFile instance
ObjFile* file;
ObjFile* fineFile;

// Transformation matrices
mat4 rot = mat4(1.0f);
mat4 scaling = mat4(1.0f);
mat4 translation = mat4(1.0f);
float scalar = 1.0f;

//shader constants
const float amplitude = 0.06125;
const float frequency = 4;
//const float amplitude = 0.2;
//const float frequency = 2;
const float PI = 3.14159;

float PHI = 1.61803398874989484820459;  // = Golden Ratio   

float gold_noise(vec2 xy, float seed) {
	return fract(tan(distance(xy * PHI, xy) * seed) * xy.x);
}

// Constants to help with location bindings
#define VERTEX_DATA 0
#define VERTEX_NORMAL 1

const unsigned int width = 1920;
const unsigned int height = 1080;


const int NUM_X = 80; //total quads on X axis
const int NUM_Z = 80; //total quads on Z axis

const float SIZE_X = 4; //size of plane in world space
const float SIZE_Z = 4;
const float HALF_SIZE_X = SIZE_X / 2.0f;
const float HALF_SIZE_Z = SIZE_Z / 2.0f;

//ripple displacement speed
const float SPEED = 2;

//ripple mesh vertices and indices
glm::vec3 vertexes[(NUM_X + 1) * (NUM_Z + 1)];
const int TOTAL_INDICES = NUM_X * NUM_Z * 2 * 3;
GLuint indices[TOTAL_INDICES];


// Vertices coordinates
//GLfloat vertices[] =
//{ //     COORDINATES     /        COLORS      /   TexCoord  //
//	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
//	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
//	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
//	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
//	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
//};

// Indices for vertices order
//GLuint indices[] =
//{
//	0, 1, 2,
//	0, 2, 3,
//	0, 1, 4,
//	1, 2, 4,
//	2, 3, 4,
//	3, 0, 4
//};


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
	glUseProgram(myShaderProgram);

	//Scale based on input
	scaling = scale(mat4(1.0f), vec3(scalar)) * scaling;

	//Create and pass model view matrix
	mat4 modelView = lookAt(vec3(0.0f, 0.0f, -10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	modelView = rot * scaling * translation;
	mat4 model = rot * scaling * translation;
	glUniformMatrix4fv(glGetUniformLocation(myShaderProgram, "model"), 1, GL_FALSE, value_ptr(model));

	//Create and pass projection matrix
	mat4 proj = perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(myShaderProgram, "proj_matrix"), 1, GL_FALSE, value_ptr(proj));

	//Set shader uniforms
	glUniform3f(glGetUniformLocation(myShaderProgram, "light_pos"), 1.0f, 1.0f, -1.0f);


	// Note that this version of the draw command uses the
	// bound index buffer to get the vertex coordinates.
	file->draw(VERTEX_DATA, VERTEX_NORMAL);
}


void setupRenderingContextFineMesh() {
	// Background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// First setup the shaders
	//Now, let's setup the shaders
	GLuint hVertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	fineMeshShader = (GLuint)NULL;
	GLint testVal;

	if (!loadShaderFile(vsFineFilename.c_str(), hVertexShader)) {
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		cout << "The shader " << vsFineFilename << " could not be found." << endl;
	}

	if (!loadShaderFile(fsFineFilename.c_str(), hFragmentShader)) {
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		cout << "The shader " << fsFineFilename << " could not be found." << endl;
	}

	glCompileShader(hVertexShader);
	glCompileShader(hFragmentShader);

	// Check for any error generated during shader compilation
	glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(hVertexShader, 8192, NULL, source);
		glGetShaderInfoLog(hVertexShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
	}
	glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(hFragmentShader, 8192, NULL, source);
		glGetShaderInfoLog(hFragmentShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
	}

	// Create the shader program and bind locations for the vertex
	// attributes before linking. The linking process can also generate errors

	fineMeshShader= glCreateProgram();
	glAttachShader(fineMeshShader, hVertexShader);
	glAttachShader(fineMeshShader, hFragmentShader);

	glBindAttribLocation(fineMeshShader, VERTEX_DATA, "position");
	//glBindAttribLocation( myShaderProgram, VERTEX_COLOUR, "vColor" );
	glBindAttribLocation(fineMeshShader, VERTEX_NORMAL, "normal");

	glLinkProgram(fineMeshShader);
	glDeleteShader(hVertexShader);
	glDeleteShader(hFragmentShader);
	glGetProgramiv(fineMeshShader, GL_LINK_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char infoLog[1024];
		glGetProgramInfoLog(fineMeshShader, 1024, NULL, infoLog);
		cout << "The shader program (" << vsFineFilename << " + " << fsFineFilename << ") failed to link:" << endl << (const char*)infoLog << endl;
		glDeleteProgram(fineMeshShader);
		fineMeshShader = (GLuint)NULL;
	}

	// Now setup the geometry in a vertex buffer object

	// setup the vertex state array object. All subsequent buffers will
	// be bound to it.
	glGenVertexArrays(1, &myFineMeshVAO);
	glBindVertexArray(myFineMeshVAO);
}


// This function does any needed initialization on the rendering
// context. 
void setupRenderingContext() {
	// Background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// First setup the shaders
	//Now, let's setup the shaders
	GLuint hVertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	myShaderProgram = (GLuint)NULL;
	GLint testVal;

	if (!loadShaderFile(vsFilename.c_str(), hVertexShader)) {
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		cout << "The shader " << vsFilename << " could not be found." << endl;
	}

	if (!loadShaderFile(fsFilename.c_str(), hFragmentShader)) {
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		cout << "The shader " << fsFilename << " could not be found." << endl;
	}

	glCompileShader(hVertexShader);
	glCompileShader(hFragmentShader);

	// Check for any error generated during shader compilation
	glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(hVertexShader, 8192, NULL, source);
		glGetShaderInfoLog(hVertexShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
	}
	glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(hFragmentShader, 8192, NULL, source);
		glGetShaderInfoLog(hFragmentShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
	}

	// Create the shader program and bind locations for the vertex
	// attributes before linking. The linking process can also generate errors

	myShaderProgram = glCreateProgram();
	glAttachShader(myShaderProgram, hVertexShader);
	glAttachShader(myShaderProgram, hFragmentShader);

	glBindAttribLocation(myShaderProgram, VERTEX_DATA, "position");
	//glBindAttribLocation( myShaderProgram, VERTEX_COLOUR, "vColor" );
	glBindAttribLocation(myShaderProgram, VERTEX_NORMAL, "normal");

	glLinkProgram(myShaderProgram);
	glDeleteShader(hVertexShader);
	glDeleteShader(hFragmentShader);
	glGetProgramiv(myShaderProgram, GL_LINK_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char infoLog[1024];
		glGetProgramInfoLog(myShaderProgram, 1024, NULL, infoLog);
		cout << "The shader program (" << vsFilename << " + " << fsFilename << ") failed to link:" << endl << (const char*)infoLog << endl;
		glDeleteProgram(myShaderProgram);
		myShaderProgram = (GLuint)NULL;
	}

	// Now setup the geometry in a vertex buffer object

	// setup the vertex state array object. All subsequent buffers will
	// be bound to it.
	glGenVertexArrays(1, &myVAO);
	glBindVertexArray(myVAO);
}


int main(int argc, char** argv)
{
	if (argc != 2) {
		cout << "Usage: " << argv[0] << " filename\n";
		exit(EXIT_FAILURE);
	}

	file = new ObjFile(argv[1]);
	if (file->numVertices() == 0) {
		cout << "Could not load file " << argv[1] << ".\n";
		exit(EXIT_FAILURE);
	}

	fineFile = new ObjFile(argv[1]);
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


	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");
	Shader groundShader("ground.vert", "ground.frag");
	// Shadow Mapping Shaders
	Shader simpleDepthShader("shadow.vert", "shadow.frag");
	Shader debugDepthShader("shadowDebug.vert", "shadowDebug.frag");



	



	float vertGround[] = {
		// positions          // colors           // texture coords
			  2.0f,  -1.5f, 2.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
			  2.0f, -1.5f, -2.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
			 -2.0f, -1.5f, -2.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
			 -2.0f,  -1.5f, 2.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indexGround[] = {
	   0, 1, 3, // first triangle
	   1, 2, 3  // second triangle
	};
	VAO VAO1;
	VAO1.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(vertGround, sizeof(vertGround));
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(indexGround, sizeof(indexGround));

	// Links VBO attributes such as coordinates and colors to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertGround), vertGround, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexGround), indexGround, GL_STATIC_DRAW);
	
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	//// Original code from the tutorial
	Texture brickTex("textures/vlziefgfw_2K_Albedo.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	brickTex.texUnit(groundShader, "tex0", 0);
	//
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//
	//// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
	//
	//// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));
	//
	//// Shadow Mapping Stuff
	//unsigned int depthMapFBO;
	//glGenFramebuffers(1, &depthMapFBO);
	//
	//const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	//
	//// Depth map for shadowMappings
	//unsigned int depthMap;
	//glGenTextures(1, &depthMap);
	//glBindTexture(GL_TEXTURE_2D, depthMap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
	//	SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//
	//glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);








	//debugDepthShader.Activate();
	//glUniform1i(glGetUniformLocation(debugDepthShader.ID, "DepthMap"), 0);

	setupRenderingContext();
	file->calculateNormals();
	glGenBuffers(1, &file->vertexBuffer);
	glGenBuffers(1, &file->indexBuffer);
	file->bufferData();
	scaling = file->getFitScale();
	translation = file->getFitTranslate();
	
	setupRenderingContextFineMesh();
	fineFile->calculateNormals();
	glGenBuffers(1, &fineFile->vertexBuffer);
	glGenBuffers(1, &fineFile->indexBuffer);
	fineFile->bufferData();
	scaling = fineFile->getFitScale();
	translation = fineFile->getFitTranslate();

	for (int i = 0; i < fineFile->vertices.size(); i += 4) {
		fineFile->vertices[i + 1] = -1.4f;
	}
	fineFile->calculateNormals();
	fineFile->bufferData();


	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		//for (int i = 0; i < (NUM_X + 1) * (NUM_Z + 1) * 8; i) {
		//	//get the Euclidean distance of the current vertex from the center of the mesh
		//	float distance = glm::length(glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]));
		//	//create a sin function using the distance, multiply frequency and add the elapsed time
		//	float y = amplitude * sin(-PI * distance * frequency + glfwGetTime());
		//	vertices[i + 1] += glfwGetTime();
		//	i += 8;
		//}

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Shadow mapping Stuff
		/*
		float near_plane = 1.0f, far_plane = 10.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		//glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(glm::vec3(0.0f,4.0f, 0.1f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		simpleDepthShader.Activate();
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.ID, "lightSpaceMatrix"),1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

		
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			brickTex.Bind();
			glm::mat4 model = glm::mat4(1.0f);
			glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		debugDepthShader.Activate();
		glUniform1f(glGetUniformLocation(debugDepthShader.ID, "near_plane"), near_plane);
		glUniform1f(glGetUniformLocation(debugDepthShader.ID, "far_plane"), far_plane);
		//debugDepthShader.setFloat("near_plane", near_plane);
		//debugDepthQuad.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderQuad();
		*/



		glBindVertexArray(myFineMeshVAO);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(fineMeshShader);

		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.Matrix(45.0f, 0.1f, 100.0f, fineMeshShader, "camMatrix");

		//Scale based on input
		scaling = scale(mat4(1.0f), vec3(scalar)) * scaling;

		//Create and pass model view matrix
		mat4 modelView = lookAt(vec3(0.0f, 0.0f, -10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		modelView = rot * scaling * translation;
		mat4 model = rot * scaling * translation;
		glUniformMatrix4fv(glGetUniformLocation(fineMeshShader, "model"), 1, GL_FALSE, value_ptr(model));

		//Create and pass projection matrix
		mat4 proj = perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(fineMeshShader, "proj_matrix"), 1, GL_FALSE, value_ptr(proj));

		//Set shader uniforms
		glUniform3f(glGetUniformLocation(fineMeshShader, "light_pos"), 1.0f, 1.0f, -1.0f);


		// Note that this version of the draw command uses the
		// bound index buffer to get the vertex coordinates.
		fineFile->draw(VERTEX_DATA, VERTEX_NORMAL);

		glBindVertexArray(0);


		
		// Tell OpenGL which Shader Program we want to use
		//shaderProgram.Activate();
		//std::cout << glfwGetTime() << "\n";
		glUseProgram(myShaderProgram);
		//glUniform1f(glGetUniformLocation(shaderProgram.ID, "time"), glfwGetTime());
		float time = glfwGetTime();
		for (int i = 0; i < file->vertices.size(); i += 4) {
			float x = file->vertices[i];
			float y = file->vertices[i + 1];
			float z = file->vertices[i + 2];
			float dist = glm::length(glm::vec3(x, 0, z));
			file->vertices[i + 1] = amplitude / 1.5 * sin(-PI * dist * frequency + time * 2) * sin(-PI * x * frequency * 50 + time * 2) * sin(-PI * x * frequency + time * 3) * sin(-PI * z * frequency / 2 + time * 4) * gold_noise(vec2(10.f), 12);
			file->vertices[i + 1] += amplitude * sin(-PI * x * z * frequency / 8 + time * 2);
			file->vertices[i + 1] += amplitude * sin(-PI * x * frequency / 16 + time * 2);
			file->vertices[i + 1] += amplitude / 10 * sin(-PI * dist * frequency * 5 + time * 2) * gold_noise(vec2(10.f), 100);
		}
		file->calculateNormals();
		file->bufferData();
		// Handles camera inputs
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.Matrix(45.0f, 0.1f, 100.0f, myShaderProgram, "camMatrix");
		glBindVertexArray(myVAO);
		renderScene();
		glBindVertexArray(0);
		








		//// Binds texture so that is appears in rendering
		//brickTex.Bind();
		//// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		//// Draw primitives, number of indices, datatype of indices, index of indices
		//glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		//// Swap the back buffer with the front buffer
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		groundShader.Activate();
		brickTex.Bind();
		//glBindVertexArray(VAO);
		camera.Matrix(45.0f, 0.1f, 100.0f, groundShader.ID, "camMatrix");
		////glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//
		////simpleDepthShader.use();
		////simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		VAO1.Unbind();

		
		



		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}



	// Delete all the objects we've created
	//brickTex.Delete();
	//shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}



// REFERENCES:
/*
https://learnopengl.com/Getting-started/Textures 
https://subscription.packtpub.com/book/game-development/9781849695046/1/ch01lvl1sec12/doing-a-ripple-mesh-deformer-using-the-vertex-shader
https://www.gamasutra.com/view/feature/2811/inexpensive_underwater_caustics_.php?print=1
https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping 
*/