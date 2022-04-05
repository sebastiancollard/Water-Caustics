
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
GLuint waterShader;
GLuint causticShader;

// Shader filenames
string waterVSFilename = "default.vert";
string waterFSFilename = "default.frag";

string causticVSFilename = "fineMesh.vert";
string causticFSFilename = "fineMesh.frag";

// ObjFile instance
ObjFile* waterMesh;
ObjFile* causticMesh;

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

	//Scale based on input
	scaling = scale(mat4(1.0f), vec3(scalar)) * scaling;

	//Create and pass model view matrix
	mat4 modelView = lookAt(vec3(0.0f, 0.0f, -10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	modelView = rot * scaling * translation;
	mat4 model = rot * scaling * translation;
	glUniformMatrix4fv(glGetUniformLocation(waterShader, "model"), 1, GL_FALSE, value_ptr(model));

	//Create and pass projection matrix
	mat4 proj = perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(waterShader, "proj_matrix"), 1, GL_FALSE, value_ptr(proj));

	//Set shader uniforms
	glUniform3f(glGetUniformLocation(waterShader, "light_pos"), 1.0f, 1.0f, -1.0f);


	// Note that this version of the draw command uses the
	// bound index buffer to get the vertex coordinates.
	waterMesh->draw(VERTEX_DATA, VERTEX_NORMAL);
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
	GLuint causticVertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint causticFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	causticShader = (GLuint)NULL;
	waterShader = (GLuint)NULL;
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

	if (!loadShaderFile(causticVSFilename.c_str(), causticVertexShader)) {
		glDeleteShader(causticVertexShader);
		glDeleteShader(causticFragmentShader);
		cout << "The shader " << causticVSFilename << " could not be found." << endl;
	}

	if (!loadShaderFile(causticFSFilename.c_str(), causticFragmentShader)) {
		glDeleteShader(causticVertexShader);
		glDeleteShader(causticFragmentShader);
		cout << "The shader " << causticFSFilename << " could not be found." << endl;
	}

	glCompileShader(waterVertexShader);
	glCompileShader(waterFragmentShader);
	glCompileShader(causticVertexShader);
	glCompileShader(causticFragmentShader);

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

	glGetShaderiv(causticVertexShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(causticVertexShader, 8192, NULL, source);
		glGetShaderInfoLog(causticVertexShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(causticVertexShader);
		glDeleteShader(causticFragmentShader);
	}
	glGetShaderiv(causticFragmentShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(causticFragmentShader, 8192, NULL, source);
		glGetShaderInfoLog(causticFragmentShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(causticVertexShader);
		glDeleteShader(causticFragmentShader);
	}

	// Create the shader program and bind locations for the vertex
	// attributes before linking. The linking process can also generate errors

	waterShader = glCreateProgram();
	glAttachShader(waterShader, waterVertexShader);
	glAttachShader(waterShader, waterFragmentShader);

	glBindAttribLocation(waterShader, VERTEX_DATA, "position");
	//glBindAttribLocation( myShaderProgram, VERTEX_COLOUR, "vColor" );
	glBindAttribLocation(waterShader, VERTEX_NORMAL, "normal");

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

	causticShader = glCreateProgram();
	glAttachShader(causticShader, causticVertexShader);
	glAttachShader(causticShader, causticFragmentShader);

	glBindAttribLocation(causticShader, VERTEX_DATA, "position");
	//glBindAttribLocation( myShaderProgram, VERTEX_COLOUR, "vColor" );
	glBindAttribLocation(causticShader, VERTEX_NORMAL, "normal");

	glLinkProgram(causticShader);
	glDeleteShader(causticVertexShader);
	glDeleteShader(causticFragmentShader);
	glGetProgramiv(causticShader, GL_LINK_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char infoLog[1024];
		glGetProgramInfoLog(causticShader, 1024, NULL, infoLog);
		cout << "The shader program (" << causticVSFilename << " + " << causticFSFilename << ") failed to link:" << endl << (const char*)infoLog << endl;
		glDeleteProgram(causticShader);
		causticShader = (GLuint)NULL;
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

	waterMesh = new ObjFile(argv[1]);
	if (waterMesh->numVertices() == 0) {
		cout << "Could not load file " << argv[1] << ".\n";
		exit(EXIT_FAILURE);
	}

	causticMesh = new ObjFile(argv[1]);
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
	Shader groundShader("ground.vert", "ground.frag");



	



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
	VAO groundVAO;
	groundVAO.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO groundVBO(vertGround, sizeof(vertGround));
	// Generates Element Buffer Object and links it to indices
	EBO groundEBO(indexGround, sizeof(indexGround));

	// Links VBO attributes such as coordinates and colors to VAO
	groundVAO.LinkAttrib(groundVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	groundVAO.LinkAttrib(groundVBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	groundVAO.LinkAttrib(groundVBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	groundVAO.Unbind();
	groundVBO.Unbind();
	groundEBO.Unbind();
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

	//Texture environmentMap("textures/EnvironmentMap.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
	//environmentMap.texUnit(fineMeshShader, "tex0", 0);
	
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int widthI, heightI, nrChannels;
	
	unsigned char* data = stbi_load("./textures/EnvironmentMap.jpg", &widthI, &heightI, &nrChannels, 0);
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
	


	

	//// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
	//// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	setupRenderingContext();
	waterMesh->calculateNormals();
	glGenBuffers(1, &waterMesh->vertexBuffer);
	glGenBuffers(1, &waterMesh->indexBuffer);
	waterMesh->bufferData();
	scaling = waterMesh->getFitScale();
	translation = waterMesh->getFitTranslate();
	
	//setupRenderingContextFineMesh();
	causticMesh->calculateNormals();
	glGenBuffers(1, &causticMesh->vertexBuffer);
	glGenBuffers(1, &causticMesh->indexBuffer);
	causticMesh->bufferData();
	scaling = causticMesh->getFitScale();
	translation = causticMesh->getFitTranslate();

	for (int i = 0; i < causticMesh->vertices.size(); i += 4) {
		causticMesh->vertices[i + 1] = -1.48f;
	}
	causticMesh->calculateNormals();
	causticMesh->bufferData();


	// Main while loop
	while (!glfwWindowShouldClose(window))
	{

		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// DRAW GROUND FIRST


		//// Binds texture so that is appears in rendering
		//brickTex.Bind();
		//// Bind the VAO so OpenGL knows to use it
		groundVAO.Bind();
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
		groundVAO.Unbind();

		
		


		// DRAW CAUSTICS SECOND
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		for (int i = 0; i < waterMesh->normals.size(); i++) {
			causticMesh->normals[i] = waterMesh->normals[i];
		}
		causticMesh->bufferData();

		glBindVertexArray(myVAO);

		glUseProgram(causticShader);

		glUniform1i(glGetUniformLocation(causticShader, "texture1"), 0);

		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.Matrix(45.0f, 0.1f, 100.0f, causticShader, "camMatrix");

		//Scale based on input
		scaling = scale(mat4(1.0f), vec3(scalar)) * scaling;

		//Create and pass model view matrix
		mat4 modelView = lookAt(vec3(0.0f, 0.0f, -10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		modelView = rot * scaling * translation;
		mat4 model = rot * scaling * translation;
		glUniformMatrix4fv(glGetUniformLocation(causticShader, "model"), 1, GL_FALSE, value_ptr(model));

		//Create and pass projection matrix
		mat4 proj = perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(causticShader, "proj_matrix"), 1, GL_FALSE, value_ptr(proj));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		//glUniform1fv(glGetUniformLocation(fineMeshShader, "samples"), 6561, &(file->normals[0]));

		//Set shader uniforms
		//glUniform3f(glGetUniformLocation(fineMeshShader, "light_pos"), 1.0f, 1.0f, -1.0f);

		// Note that this version of the draw command uses the
		// bound index buffer to get the vertex coordinates.
		causticMesh->draw(VERTEX_DATA, VERTEX_NORMAL);

		glBindVertexArray(0);




		// DRAW WAVE LAST, FOR BLENDING/TRANSPARENCY

		// Tell OpenGL which Shader Program we want to use
		//shaderProgram.Activate();
		//std::cout << glfwGetTime() << "\n";
		glUseProgram(waterShader);
		//glUniform1f(glGetUniformLocation(shaderProgram.ID, "time"), glfwGetTime());
		float time = glfwGetTime();
		for (int i = 0; i < waterMesh->vertices.size(); i += 4) {
			float x = waterMesh->vertices[i];
			float y = waterMesh->vertices[i + 1];
			float z = waterMesh->vertices[i + 2];
			float dist = glm::length(glm::vec3(x, 0, z));
			waterMesh->vertices[i + 1] = amplitude / 1.5 * sin(-PI * dist * frequency + time * 2) * sin(-PI * x * frequency * 50 + time * 2) * sin(-PI * x * frequency + time * 3) * sin(-PI * z * frequency / 2 + time * 4) * gold_noise(vec2(10.f), 12);
			waterMesh->vertices[i + 1] += amplitude * sin(-PI * x * z * frequency / 8 + time * 2);
			waterMesh->vertices[i + 1] += amplitude * sin(-PI * x * frequency / 16 + time * 2);
			waterMesh->vertices[i + 1] += amplitude / 10 * sin(-PI * dist * frequency * 5 + time * 2) * gold_noise(vec2(10.f), 100);
		}
		waterMesh->calculateNormals();
		waterMesh->bufferData();
		// Handles camera inputs
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.Matrix(45.0f, 0.1f, 100.0f, waterShader, "camMatrix");
		glUniform3fv(glGetUniformLocation(waterShader, "viewPos"), 1, &camera.Position[0]);
		
		glBindVertexArray(myVAO);
		renderScene();
		glBindVertexArray(0);


		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();

		//std::cout << "Normals: " << file->normals.size()<< "\n";
		//std::cout << "Vertices: " << file->vertices.size() << "\n";

		//std::cout << "Normals1: " << fineFile->normals.size() << "\n";
		//std::cout << "Vertices1: " << fineFile->vertices.size() << "\n";
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