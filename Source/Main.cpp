#include <GLFW/glfw3.h>
#include <stdio.h>
#include "GameActor.hpp"
#include <nima/Animation/ActorAnimation.hpp>
#include <nima/Exceptions/OverflowException.hpp>
#include <nima/Exceptions/UnsupportedVersionException.hpp>
#include <nima/Animation/Interpolators/CubicSolver.hpp>
#include "Graphics/Bitmap/Bitmap.hpp"
#include "Graphics/OpenGL/GLRenderer2D.hpp"
#include "ArcherController.hpp"


// The input is handled globally in this example for the sake of simplicity and clarity.
nima::Vec2D screenMouse;

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
	screenMouse[0] = (float)xpos;
	screenMouse[1] = (float)ypos;
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

	glfwSetErrorCallback(error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	int initialWindowWidth = 640;
	int initialWindowHeight = 480;

	nima::Vec2D m_ViewCenter(0.0f, 0.0f);
	float m_CameraScale = 0.2;
	nima::Mat2D viewTransform;
	nima::Mat2D inverseViewTransform;

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

	// Init renderer.
	nima::Renderer2D* renderer = new nima::GLRenderer2D();


	nima::GameActor* actor = new nima::GameActor();

	try
	{
		actor->load("Assets/Archer.nima");
	}
	catch (nima::OverflowException ex)
	{
		printf("Bad data, got an overflow.\n");
	}
	catch (nima::UnsupportedVersionException ex)
	{
		printf("Unsupported version. %d %d\n", ex.versionFound(), ex.versionRequired());
	}

	actor->initializeGraphics(renderer);

	nima::GameActorInstance* actorInstance = actor->makeInstance();
	actorInstance->initializeGraphics(renderer);

	ArcherController* characterController = actorInstance->addController<ArcherController>();

	int width = 0, height = 0;
	int lastScreenWidth = width, lastScreenHeight = height;
	double lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &width, &height);
		if (width != lastScreenWidth || height != lastScreenHeight)
		{
			lastScreenWidth = width;
			lastScreenHeight = height;
			// resized.
			renderer->setViewportSize(width, height);

			m_ViewCenter[1] = height*2.0f;

			viewTransform[0] = m_CameraScale;
			viewTransform[3] = m_CameraScale;
			viewTransform[4] = (-m_ViewCenter[0] * m_CameraScale + width/2.0f);
			viewTransform[5] = (-m_ViewCenter[1] * m_CameraScale + height/2.0f);
			renderer->setView(viewTransform);

			nima::Mat2D::invert(inverseViewTransform, viewTransform);
		}

		nima::Vec2D worldMouse(screenMouse[0], lastScreenHeight - screenMouse[1]);
		nima::Vec2D::transform(worldMouse, worldMouse, inverseViewTransform);

		double time = glfwGetTime();
		float elapsed = (float)(time - lastTime);
		lastTime = time;

		characterController->setAimTarget(worldMouse);
		actorInstance->advance(elapsed);

		renderer->clear();
		actorInstance->render(renderer);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup.
	delete actorInstance;
	delete actor;
	delete renderer;

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
