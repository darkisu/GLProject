
// Custom Class Includes
#include "Scene.h"
#include "Camera.h"

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

// Window
GLuint screenWidth = 800;
GLuint screenHeight = 600;


// Display Object
GLFWwindow* pWindow;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


GLfloat scaleVal = 0.1f;

int main()
{

	initiate( screenWidth, screenHeight,"GLProject Result");

	//Model TargetModel(".\\model\\nanosuit\\nanosuit.obj");
	Scene TargetScene(".\\model\\sponza.obj");
	//Model TargetModel2(".\\model\\cube\\cube.obj");
	//Model TargetModel2(".\\model\\plane\\cube.obj");
	Shader shader("./shader/Shader.vs", "./shader/Shader.fs");



	//------Main Loop------//
	while (!glfwWindowShouldClose(pWindow))
	{
		GLfloat currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrame;
		lastFrame = currentFrameTime;



		// Check and call events
		glfwPollEvents();
		Do_Movement();

		// Clear the colorbuffer
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Use();   // <-- Don't forget this one!
						// Transformation matrices
		//------------------Render Main Model-----------------------------

		glUniform3fv(glGetUniformLocation(shader.Program, "ViewPos"), 1, glm::value_ptr(camera.Position));
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

		// Draw the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		//model = glm::scale(model, glm::vec3(scaleVal, scaleVal, scaleVal));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		TargetScene.setupPointOfLight(glm::vec3(0.0, 1000.0, 0.0), glm::vec3(1.0, 1.0, 1.0));

		TargetScene.Draw(shader);


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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
	//camera.ProcessMouseScroll(yoffset);
	scaleVal *= 1+yoffset / abs(yoffset)*0.2;
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
}