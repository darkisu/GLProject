#define _USE_MATH_DEFINES
#include <cmath>
// Custom Class Includes
#include "Scene.h"
#include "Camera.h"
#include "TextureShower.h"
#include "DeferredRenderer.h"
#include "ReflectiveShadowMap.h"
#include "LPV.h"
// GL Includes
#include <GLEW\glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// window initiation
void initiate(int width, int height, string title);

// callbacks
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void Do_Movement();
void Process_Light();


// Window
GLuint screenWidth = 800;
GLuint screenHeight = 600;


// Display Object
GLFWwindow* pWindow;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));

bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Point of Light
typedef struct
{
	glm::vec3 position;
	glm::vec3 color;
}PointOfLight;


// Point of Light
PointOfLight lightsource;
const float lightRange = 3064;
const float lightmovspeed = 0.01;
GLfloat lighttheta = 0, lightphi = M_PI / 2.0;
bool lightMoved = true;


int main()
{

	initiate( screenWidth, screenHeight,"GLProject Result");
	// Scene ID Variables
	GLuint sponzaID, cubeID;

	// Scenes
	Scene TargetScene(".\\model\\sponza.obj", sponzaID);
	//Scene TargetScene(".\\model\\MyScene98.obj", sponzaID);
	//TargetScene.loadModel(".\\model\\sponza.obj", cubeID);
	//TargetScene.setModel(sponzaID, true, glm::vec3(0, 0, 0), glm::vec3(1.0, 1.0, 1.0));
	
	//Scene TargetScene(".\\model\\san-miguel.obj",sponzaID);

	// Shaders
	Shader shader("./shader/defaultshader.glvs", "./shader/defaultshader.glfs",SHADER_FROM_FILE);
	//Shader shader("./shader/deferredp1.glvs", "./shader/deferredp1.glfs", SHADER_FROM_FILE);
	Shader showTexShader("./shader/showTexture.glvs", "./shader/showTexture.glfs", SHADER_FROM_FILE);
	Shader RSMshader("./shader/RSM.glvs", "./shader/RSM.glfs", SHADER_FROM_FILE);

	// Deferred Renderer
	DeferredRenderer renderer(&TargetScene, screenWidth, screenHeight);

	// Texture Shower
	TextureShower textureShower;
	textureShower.setTexture(renderer.diffuseTex);


	// Reflective Shadow Map
	ReflectiveShadowMap RSM(256,&TargetScene);

	// Point of Light
	lightsource.position = glm::vec3(0.0, lightRange, 0.0);
	lightsource.color = glm::vec3(1, 1, 1);



	//LPV
	LPV lpv(&TargetScene, &RSM);


	//------Main Loop------//
	while (!glfwWindowShouldClose(pWindow))
	{
		GLfloat currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrame;
		lastFrame = currentFrameTime;

		// Check and call events
		glfwPollEvents();
		Do_Movement();
		Process_Light();

		//RSM
		RSM.draw(RSMshader, lightsource.position);
		glViewport(0, 0, screenWidth, screenHeight);

		//LPV
		if (lightMoved)
		{
			lpv.gather();
			lpv.inject(lightsource.position);   // cause the slowing down of the rendering because of the loops in shader
			lightMoved = false;
		}
		lpv.propagate();


		// Clear the colorbuffer
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glActiveTexture(GL_TEXTURE0);

		// use shader
		shader.Use();

		// Camera setup
		glViewport(0, 0, screenWidth, screenHeight);
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 1.0f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 VPMatrix = projection * view;

		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "VPMatrix"), 1, GL_FALSE, glm::value_ptr(VPMatrix));
		glUniform3fv(glGetUniformLocation(shader.Program, "ViewPos"), 1, glm::value_ptr(camera.Position));
		// Setup Light
		glUniform3fv(glGetUniformLocation(shader.Program, "LightPos"), 1, glm::value_ptr(lightsource.position));
		glUniform3fv(glGetUniformLocation(shader.Program, "LightColor"), 1, glm::value_ptr(lightsource.color));


	
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "depthVP"), 1, GL_FALSE, glm::value_ptr(RSM.getShadowMappingMatrix() ));
		// Shadow Texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, RSM.shadowMapTextureID);
		glUniform1i(glGetUniformLocation(shader.Program, "depthTexture"), 1);
		
		// LPV SHs
		for (int i = 0; i < lpv.depth; i++)
		{
			glActiveTexture(GL_TEXTURE2 + i);
			glBindTexture(GL_TEXTURE_3D, lpv.lightInfo3D[i]);
		}
		glUniform1i(glGetUniformLocation(shader.Program, "LPV00"), 2);
		glUniform1i(glGetUniformLocation(shader.Program, "LPVn11"), 3);
		glUniform1i(glGetUniformLocation(shader.Program, "LPV01"), 4);
		glUniform1i(glGetUniformLocation(shader.Program, "LPVp11"), 5);


		glUniform3fv(glGetUniformLocation(shader.Program, "lowerbound"), 1, glm::value_ptr(TargetScene.bCubeLower));
		glUniform1f(glGetUniformLocation(shader.Program, "bboxlength"), TargetScene.bCubeLength);
		glUniform1f(glGetUniformLocation(shader.Program, "LPVres"), lpv.resolution);
		glUniform1f(glGetUniformLocation(shader.Program, "LPVsize"), lpv.LPVSize);
		glUniform3fv(glGetUniformLocation(shader.Program, "eyepoint"), 1, glm::value_ptr(camera.Position));

		// Draw the loaded model by deferred renderer

		TargetScene.Draw(shader);
		//renderer.drawP1(shader);


		// Show Texture
		//textureShower.setTexture(renderer.diffuseTex);
		//textureShower.showTexture(showTexShader);

		glBindTexture(GL_TEXTURE_2D, 0);
		glfwSwapBuffers(pWindow);
	}


	return 0;
}

void initiate(int width, int height, string title)
{
	//------GLFW Initiation------//
	if (glfwInit() == false) {
		cerr << "ERROR: GLFW initialization failed! -- (glfwinit)" << endl;
		system("PAUSE");
		glfwTerminate();
	}
	glfwWindowHint(GLFW_SAMPLES, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//------GLFW Window Creating------//
	pWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if (pWindow == NULL)
	{
		cerr << "ERROR: Main window creation failed! -- (glfwCreateWindow)" << endl;
		system("PAUSE");
		glfwTerminate();
	}
	glfwMakeContextCurrent(pWindow);

	//------GLFW Options------//
	glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(pWindow, key_callback);
	glfwSetCursorPosCallback(pWindow, mouse_callback);
	glfwSetScrollCallback(pWindow, scroll_callback);


	//enable glew for newer version of open GL
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		cerr << "ERROR: GLEW initialization failed! -- (glewInit)" << endl;
		system("PAUSE");
	}

	//------Viewport Definition------//
	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void Do_Movement() 
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_LEFT])
	{
		lighttheta -= lightmovspeed;
		lightMoved = true;
	}
	if (keys[GLFW_KEY_RIGHT])
	{
		lighttheta += lightmovspeed;
		lightMoved = true;
	}
	if (keys[GLFW_KEY_UP])
	{
		lightphi += lightmovspeed;
		lightMoved = true;
	}
	if (keys[GLFW_KEY_DOWN])
	{
		lightphi -= lightmovspeed;
		lightMoved = true;
	}
}



void Process_Light()
{
	lightphi = lightphi > M_PI / 2.0 ? M_PI / 2.0 : lightphi;
	lightphi = lightphi < 0 ? 0 : lightphi;
	lighttheta = lighttheta > 2 * M_PI ? lighttheta - (2 * M_PI) : lighttheta;
	lightsource.position.y = lightRange * sin(lightphi);
	lightsource.position.x = lightRange * cos(lightphi) * cos(lighttheta);
	lightsource.position.z = lightRange * cos(lightphi) * sin(lighttheta);
}