
#include "GL/glew.h"
#include <cstdlib>
#include <cstdio>
#include <GLFW/glfw3.h>

#include "SceneShader.h"

double mouse_old_x, mouse_old_y;
int mouse_buttons = 0;
float rotate_x = 0.0, rotate_y = 0.0;
float translate_z = -1.0;

int width = 1024;
int height = 1024;
GLFWwindow* window;

SceneShader shader;

void keyboard( unsigned char key, int x, int y )
{
	float factor = 0.05f;

	switch (key)
	{
	case (27):
						exit(EXIT_FAILURE);
	case (GLFW_KEY_X):
						rotate_y += 1.0f;
	break;
	case (GLFW_KEY_Z):
						rotate_y -=  1.0f;
	break;

	case (GLFW_KEY_A):
				shader.updateLightPositionX(-factor);
	break;
	case (GLFW_KEY_D):
				shader.updateLightPositionX(factor);
	break;
	case (GLFW_KEY_S):
				shader.updateLightPositionY(-factor);
		break;
	case (GLFW_KEY_W):
				shader.updateLightPositionY(factor);
	break;

	case (GLFW_KEY_Q):
				shader.updateLightPositionZ(-factor);
	break;
	case (GLFW_KEY_E):
				shader.updateLightPositionZ(factor);
		break;

	default:
		break;
	}


}


void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if (action == GLFW_PRESS)
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		mouse_old_x = x;
		mouse_old_y = y;

		//when the mouse is pressed we change the coordinates to be from -1 to 1 and tell
		//all the trackbars that the mouse has been clicked
		int windowWidth, windowHeight;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		shader.mousePressed((float)x / windowWidth * 2 - 1, (float)y / windowHeight * 2 - 1);
	} else if (action == GLFW_RELEASE) {
		//when the mouse is released, we indicate that to all the trackbars
		shader.mouseReleased();
	}
}

void motion( GLFWwindow* w, double x, double y )
{
	//if we are not changing values of any trackbar, then move the scene
	if (!shader.mouseStuckToTrackBar()) {
		double dx, dy;
		dx = (x - mouse_old_x);
		dy = (y - mouse_old_y);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
		{
			rotate_x += dy * 0.5f;
			rotate_y += dx * 0.5f;
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
		{
			translate_z += dy * 0.03f;
		}
	} else {
		//otherwise we change the mouse pos to the range (-1, 1)
		//and tell the corresponding trackbar about the new mouse pos
		int windowWidth, windowHeight;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);

		TrackBar *bar = shader.getBarMouseStuckTo();
		bar->setNewMousePosition((float)x / windowWidth * 2 - 1, (float)y / windowHeight * 2 - 1);
	}

	mouse_old_x = x;
	mouse_old_y = y;
}

void scroll( GLFWwindow* w, double x, double y )
{
	double dy;
	dy = (x - y);

	translate_z -= dy * 0.03f;
}

void render( )
{
	GLfloat color[] = {0.3215f, 0.3411f, 0.4352f, 1.0f};
	const GLfloat zero = 1.0f;

	glClearBufferfv(GL_COLOR, 0, color);
	glClearBufferfv(GL_DEPTH, 0, &zero);
	glEnable(GL_DEPTH_TEST);

	shader.setRotationY(rotate_y);
	shader.setRotationX(rotate_x);
	shader.setZTranslation( translate_z );

	shader.render();

	glDisable(GL_DEPTH_TEST);
}

void reshape( GLFWwindow* w, int widthX, int heightY )
{
	width = widthX;
	height = heightY;
	glViewport(0, 0, width, height);
}

static void error_callback( int error, const char* description )
{
	fputs(description, stderr);
}

static void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	keyboard(key, 0, 0);

}

void startGlew( )
{
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong */
		fprintf(stderr, "Error: %s \n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* vendor = glGetString(GL_VENDOR); // vendor name string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	// GLSL version string
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major); // get integer (only if gl version > 3)
	glGetIntegerv(GL_MINOR_VERSION, &minor); // get dot integer (only if gl version > 3)
	printf("OpenGL on %s %s\n", vendor, renderer);
	printf("OpenGL version supported %s\n", version);
	printf("GLSL version supported %s\n", glslVersion);
	printf("GL version major, minor: %i.%i\n", major, minor);

	shader.startup();
}

int main( int argc, char**argv )
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_SAMPLES, 16);

	window = glfwCreateWindow(width, height, "OpenGL Window", NULL, NULL);

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse);
	glfwSetCursorPosCallback(window, motion);
	glfwSetScrollCallback(window, scroll);
	glfwSetWindowSizeCallback(window, reshape);

	startGlew();

	while ( !glfwWindowShouldClose(window) )
	{
		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float) height;
		shader.setAspectRatio(ratio);

		render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
