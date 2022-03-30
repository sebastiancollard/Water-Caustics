
//------- Ignore this ----------
#include<filesystem>
//------------------------------

#include<iostream>
#include<glad/glad.h>
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

void renderQuad();



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


int main()
{
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



	//setup plane geometry
	//setup plane vertices
	int count = 0;
	int i = 0, j = 0;
	for (j = 0; j <= NUM_Z; j++) {
		for (i = 0; i <= NUM_X; i++) {
			vertexes[count++] = glm::vec3(((float(i) / (NUM_X - 1)) * 2 - 1) * HALF_SIZE_X, 0, ((float(j) / (NUM_Z - 1)) * 2 - 1) * HALF_SIZE_Z);
		}
	}

	//fill plane indices array
	
	GLuint* id = &indices[0];
	for (i = 0; i < NUM_Z; i++) {
		for (j = 0; j < NUM_X; j++) {
			int i0 = i * (NUM_X + 1) + j;
			int i1 = i0 + 1;
			int i2 = i0 + (NUM_X + 1);
			int i3 = i2 + 1;
			//std::cout << "Val: " << i0 + " " << i1 << " " << i2 << " " << i3 << "\n";
			if ((j + i) % 2) {
				*id++ = i0; *id++ = i2; *id++ = i1;
				*id++ = i1; *id++ = i2; *id++ = i3;
			}
			else {
				*id++ = i0; *id++ = i2; *id++ = i3;
				*id++ = i0; *id++ = i3; *id++ = i1;
			}
		}
	}
	
	std::vector<GLfloat> verts;
	for (j = 0; j <= NUM_Z; j++) {
		for (i = 0; i <= NUM_X; i++) {
			//std::cout << "x: " << ((float(i) / (NUM_X - 1)) * 2 - 1) * HALF_SIZE_X << "\n";
			//std::cout << "z: " << ((float(j) / (NUM_Z - 1)) * 2 - 1) * HALF_SIZE_Z << "\n";
			verts.push_back(((float(i) / (NUM_X - 1)) * 2 - 1) * HALF_SIZE_X);
			verts.push_back(0);
			verts.push_back(((float(j) / (NUM_Z - 1)) * 2 - 1) * HALF_SIZE_Z);
			verts.push_back(0.83f);
			verts.push_back(0.70f);
			verts.push_back(0.44f);
			verts.push_back(0.f);
			verts.push_back(0.f);
		}
	}
	const int SIZE = verts.size();
	GLfloat vertices[(NUM_X + 1) * (NUM_Z + 1) * 8];
	for (int i = 0; i < verts.size(); i++) vertices[i] = verts[i];

	// Generates Vertex Array Object and binds it
	VAO VAO1;
	VAO1.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(vertices, sizeof(vertices));
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(indices, sizeof(indices));

	// Links VBO attributes such as coordinates and colors to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

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

	// Original code from the tutorial
	Texture brickTex("textures/brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	brickTex.texUnit(groundShader, "tex0", 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	// Shadow Mapping Stuff
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	// Depth map for shadowMappings
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);






	const float amplitude = 0.125;
	const float frequency = 4;
	const float PI = 3.14159;

	debugDepthShader.Activate();
	glUniform1i(glGetUniformLocation(debugDepthShader.ID, "DepthMap"), 0);

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

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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


		
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();
		//std::cout << glfwGetTime() << "\n";
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "time"), glfwGetTime());
		//glUniform1f(glGetUniformLocation(shaderProgram.ID, "time"), 1.0f);

		// Handles camera inputs
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

		// Binds texture so that is appears in rendering
		brickTex.Bind();
		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		// Swap the back buffer with the front buffer
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		groundShader.Activate();
		brickTex.Bind();
		glBindVertexArray(VAO);
		camera.Matrix(45.0f, 0.1f, 100.0f, groundShader, "camMatrix");
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//simpleDepthShader.use();
		//simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		






		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}



	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	brickTex.Delete();
	shaderProgram.Delete();
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