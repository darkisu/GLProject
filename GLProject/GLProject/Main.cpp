
// Custom Class Includes
#include "Scene.h"
#include "Camera.h"

// GL Includes
#include <GLEW\glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// flat square
static const GLfloat UNIT_SQUARE[] = {
	-1.0f, -1.0f, 0.0f, 0, 1,
	1.0f, -1.0f, 0.0f, 1, 1,
	-1.0f,  1.0f, 0.0f, 0, 0,
	-1.0f,  1.0f, 0.0f, 0, 0,
	1.0f, -1.0f, 0.0f, 1, 1,
	1.0f,  1.0f, 0.0f, 1, 0
};

// window initiation
void initiate(int width, int height, string title);
// callbacks
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void Do_Movement();


void showTexture(const GLuint &textureID, Shader shader);

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


int main()
{

	initiate( screenWidth, screenHeight,"GLProject Result");
	GLuint sponzaID, cubeID;
	//Model TargetModel(".\\model\\nanosuit\\nanosuit.obj");
	Scene TargetScene(".\\model\\sponza.obj", sponzaID);
	//Model TargetModel2(".\\model\\cube\\cube.obj");
	//Model TargetModel2(".\\model\\plane\\cube.obj");
	Shader shader("./shader/defaultshader.vs", "./shader/defaultshader.fs");
	Shader showTexShader("./shader/showTexture.vs", "./shader/showTexture.fs");
	//--------------test code--------------

	GLuint renderTex, depthTex;
	glGenTextures(1, &renderTex);
	GLuint FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderTex);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTex, 0);

	
	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = {  GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//-------------------------------------

	//------Main Loop------//
	while (!glfwWindowShouldClose(pWindow))
	{
		GLfloat currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrame;
		lastFrame = currentFrameTime;

		//-----------test code-------------
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glViewport(0, 0, screenWidth, screenHeight);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);



		// Check and call events
		glfwPollEvents();
		Do_Movement();

		// Clear the colorbuffer
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// use shader
		shader.Use();


		

		//------------------Render Main Model-----------------------------
		// Camera setup
		glUniform3fv(glGetUniformLocation(shader.Program, "ViewPos"), 1, glm::value_ptr(camera.Position));
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

		// Draw the loaded model
		TargetScene.setupPointOfLight(glm::vec3(0.0, 1000.0, 0.0), glm::vec3(1.0, 1.0, 1.0));

		TargetScene.Draw(shader);

		//----------test code---------
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//showTexture(renderTex,showTexShader);


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
}

void showTexture(const GLuint &textureID, Shader shader)
{
	shader.Use();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLuint quad_vertexbuffer , VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UNIT_SQUARE), UNIT_SQUARE, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1f(glGetUniformLocation(shader.Program, "renderedTexture"), 0);

	
	glDrawArrays(GL_TRIANGLES, 0, 2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
