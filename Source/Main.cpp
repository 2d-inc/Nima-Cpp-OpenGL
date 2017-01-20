#include <GLFW/glfw3.h>
#include <stdio.h>
#include "GameActor.hpp"
#include <nima/Animation/ActorAnimation.hpp>
#include <nima/Exceptions/OverflowException.hpp>
#include <nima/Exceptions/UnsupportedVersionException.hpp>
#include <nima/Animation/Interpolators/CubicSolver.hpp>
#include "Graphics/Bitmap/Bitmap.hpp"
#include "Graphics/OpenGL/GLRenderer2D.hpp"

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
	#if 0
	nima::GameActor* actor = new nima::GameActor();

	try
	{
		actor->load("Assets/IK.nima");
	}
	catch (nima::OverflowException ex)
	{
		printf("Bad data, got an overflow.\n");
	}
	catch (nima::UnsupportedVersionException ex)
	{
		printf("Unsupported version. %d %d\n", ex.versionFound(), ex.versionRequired());
	}

	nima::GameActorInstance* actorInstance = actor->makeInstance();
	nima::ActorAnimation* animation = actorInstance->getAnimation("Untitled");
	//float animationTime = 0.0f;
	if(animation == nullptr)
	{
		printf("NO ANIMATION\n");
	}
	else
	{
		
		animation->apply(0.5, actorInstance, 1.0f);
		actorInstance->advance(0.16f);
		//printf("POS %f %f\n", actorInstance->getNode("IK Target")->x(), actorInstance->getNode("IK Target")->y());
	}
	#endif
#if 1

	if (!glfwInit())
	{
		return 0;
	}

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

	actor->initialize(renderer);

	nima::GameActorInstance* actorInstance = actor->makeInstance();
	actorInstance->initialize(renderer);

	nima::ActorAnimation* animation = actorInstance->getAnimation("Aim2");
	float animationTime = 0.0f;
	if(animation == nullptr)
	{
		printf("NO ANIMATION\n");
	}
	else
	{
		printf("GOT IT\n");
	}

	int width = 0, height = 0;
	int lastWidth = width, lastHeight = height;
	double lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &width, &height);
		if (width != lastWidth || height != lastHeight)
		{
			lastWidth = width;
			lastHeight = height;
			// resized.
			renderer->setViewportSize(width, height);
		}
		renderer->clear();

		double time = glfwGetTime();
		float elapsed = (float)(time - lastTime);
		lastTime = time;


		animationTime += elapsed;
		while(animationTime > animation->duration())
		{
			animationTime -= animation->duration();
		}
		animation->apply(animationTime, actorInstance, 1.0f);
		
		actorInstance->advance(elapsed);
		actorInstance->render(renderer);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
#endif
}