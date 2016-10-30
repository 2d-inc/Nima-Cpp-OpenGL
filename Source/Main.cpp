#include <GLFW/glfw3.h>
#include <stdio.h>
#include <nima/Actor.hpp>
#include <nima/Exceptions/OverflowException.hpp>

void error_callback(int error, const char* description)
{
    puts(description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }
}

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

int main(int argc, char** argv)
{
	if(!glfwInit())
	{
		return 0;
	}
	

    try
    {
        nima::Actor* actor = nima::Actor::fromFile("Assets/Archer.nima");
        delete actor;
    }
    catch(nima::OverflowException ex)
    {
        printf("GOT EX\n");
    }
    return 0;

	glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    int initialWindowWidth = 640;
    int initialWindowHeight = 480;

	GLFWwindow* window = glfwCreateWindow(initialWindowWidth, initialWindowHeight, "Nima", NULL, NULL);
	if (!window)
	{
		// Window or OpenGL context creation failed
		printf("Failed to make window or GL.\n");
		glfwTerminate();
		return 0;
	}

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCharCallback(window, character_callback);

	glfwMakeContextCurrent(window);

	// Enable VSYNC.
	glfwSwapInterval(1);

    int width = initialWindowWidth, height = initialWindowHeight;
    int lastWidth = width, lastHeight = height;
	while (!glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &width, &height);
		if(width != lastWidth || height != lastHeight)
		{
			lastWidth = width;
			lastHeight = height;
            // resized.
		}
		glViewport(0, 0, width, height);
		glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

		glClear(GL_COLOR_BUFFER_BIT);
		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}