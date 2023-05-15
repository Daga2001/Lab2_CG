#include "Window.h"

/**
 * The function initializes the properties of a Window object, including its width, height, and key and
 * mouse input values.
 */
Window::Window()
{
	width = 800;
	height = 600;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}

	xChange = 0.0f;
	yChange = 0.0f;
	mouseFirstMoved = true;
}

/**
 * The function initializes the properties of a Window object, including its width, height, and key and
 * mouse input values.
 * 
 * @param windowWidth The width of the window in pixels.
 * @param windowHeight The height of the window in pixels.
 */
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}

	xChange = 0.0f;
	yChange = 0.0f;
	mouseFirstMoved = true;
}

/**
 * This function initializes and sets up a GLFW window with OpenGL properties and extensions.
 * 
 * @return an integer value, which is either 0 or 1 depending on whether the initialisation was
 * successful or not.
 */
int Window::Initialise()
{
	if (!glfwInit())
	{
		printf("Error Initialising GLFW");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW Windows Properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core Profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatiblity
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);

	// Create the window
	mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		printf("Error creating GLFW window!");
		glfwTerminate();
		return 1;
	}


	// Get buffer size information
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set the current context
	glfwMakeContextCurrent(mainWindow);

	//Handle key + mouse 
	createCallbacks();
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Allow modern extension access
	glewExperimental = GL_TRUE;

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		printf("Error: %s", glewGetErrorString(error));
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	//glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLineWidth(20.0f);
	glPointSize(20.0f);
	glEnable(GL_LINE_SMOOTH);

	// Create Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);

	glfwSetWindowUserPointer(mainWindow, this);
}

/**
 * This function sets the callback functions for handling keyboard and mouse events in a GLFW window.
 */
void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

/**
 * The function returns the value of xChange and resets it to 0.0f.
 * 
 * @return the value of the variable "theChange", which is the amount of change in the x-coordinate of
 * the window since the last time this function was called.
 */
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

/**
 * The function returns the value of yChange and resets it to 0.
 * 
 * @return the value of the variable "theChange", which is a GLfloat data type.
 */
GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

/**
 * The function handles keyboard input by setting flags for pressed or released keys.
 * 
 * @param window A pointer to the GLFW window that received the input event.
 * @param key The keyboard key that was pressed or released.
 * @param code The physical key code of the pressed or released key. This parameter is not used in the
 * given function.
 * @param action The action parameter in the handleKeys function is an integer that represents the
 * action that was performed on a keyboard key. It can have one of three values: GLFW_PRESS,
 * GLFW_RELEASE, or GLFW_REPEAT. GLFW_PRESS indicates that the key was pressed, GLFW_RELEASE indicates
 * that the key was released, and
 * @param mode The mode parameter is not used in this function and is therefore irrelevant.
 */
void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	// validation, in case we catch bizarre characters
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			//printf("Pressed: %d\n",key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("Released: %d\n",key);
		}
	}

}

/**
 * This function handles mouse movement in a GLFW window and calculates the change in mouse position.
 * 
 * @param window A pointer to the GLFW window that received the mouse input.
 * @param xPos The current x-coordinate of the mouse cursor.
 * @param yPos The y-coordinate of the current mouse position in the window, measured from the top-left
 * corner of the window.
 */
void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
	//printf("x:%.6f, y:%.6f\n", theWindow->xChange, theWindow->yChange);
}

/**
 * This function destroys the GLFW window and terminates the GLFW library.
 */
Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
