#include <GLFW/glfw3.h>
#include <stdio.h>
#include <nima/Actor.hpp>
#include <nima/Exceptions/OverflowException.hpp>
#include <nima/Animation/Interpolators/CubicSolver.hpp>
#include "Graphics/Bitmap/Bitmap.hpp"

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
	if (!glfwInit())
	{
		return 0;
	}

	nima::Bitmap* bmp = new nima::Bitmap();
	bmp->loadFromPNG("Assets/Archer.png", true);

	/*nima::CubicSolver solver;
	solver.set(0.0f, 34.77278518676758f,  0.8424242403224804f, 29.32800508902757f,  0.00000999999999995449f, 161.2324085038573f,  1.0f, 98.13641357421875f);
	float expected[60] = {34.77278518676758, 34.68191960114582, 34.626263045220234, 34.60825406058168, 34.630616793905936, 34.69640927316223, 34.80908258233374, 34.97255407429494, 35.19129888214371, 35.470465595669474, 35.81602431382377, 36.23495877318582, 36.73551956101267, 37.32756369914048, 38.023019154232124, 38.836534778601106, 39.78641381941253, 40.89599639959702, 42.195782600806766, 43.72683875969582, 45.54656481224143, 47.7391451822138, 50.436237728268814, 53.863166555806046, 58.46164806479278, 65.31831409552156, 78.36519515402807, 96.74933053662754, 104.50584942069626, 108.5097884695479, 110.98771097769225, 112.62997351481819, 113.73961779039637, 114.4775418713105, 114.93990868784401, 115.18903138882105, 115.26773413400494, 115.20679337895885, 115.02911889353365, 114.75225556329072, 114.38995778094773, 113.95322199445968, 113.45098765217877, 112.89062715646956, 112.27829704435263, 111.61919524907398, 110.91775319639322, 110.17778168580635, 109.4025833537627, 108.59504055052177, 107.75768484454623, 106.89275260388268, 106.00222989070684, 105.08788905743623, 104.15131882996351, 103.19394922884001, 102.2170723627827, 101.2218598927122, 100.2093777906199, 99.18059888389004};
	float inc = 1.0f / 60.0f;
	for (int i = 0; i < 60; i++)
	{
		float _x = i * inc;
		printf("VAL %i %f %f %f\n",i, _x, solver.get(_x), (expected[i] - solver.get(_x)));
	}*/

	try
	{
		nima::Actor* actor = nima::Actor::fromFile("Assets/Archer.nima");
		delete actor;
	}
	catch (nima::OverflowException ex)
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
		if (width != lastWidth || height != lastHeight)
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