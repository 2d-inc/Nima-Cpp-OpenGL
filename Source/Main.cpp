#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>
#include <unistd.h>
#include <stdio.h>
#include "GameActor.hpp"
#include <nima/Animation/ActorAnimation.hpp>
#include <nima/Exceptions/OverflowException.hpp>
#include <nima/Exceptions/UnsupportedVersionException.hpp>
#include <nima/Animation/Interpolators/CubicSolver.hpp>
#include "Graphics/Bitmap/Bitmap.hpp"
#include "Graphics/OpenGL/GLRenderer2D.hpp"
#include "ArcherController.hpp"
#include "TGAWriter.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>


// The input is handled globally in this example for the sake of simplicity and clarity.
nima::Vec2D screenMouse;
ArcherController* characterController = nullptr;
int wHeight = 303;
int wWidth = 540;

void error_callback(int error, const char* description)
{
	puts(description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_REPEAT)
	{
		return;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}

	switch(key)
	{
		case GLFW_KEY_A:
			if(characterController != nullptr)
			{
				characterController->moveLeft(action == GLFW_PRESS);
			}
			break;
		case GLFW_KEY_D:
			if(characterController != nullptr)
			{
				characterController->moveRight(action == GLFW_PRESS);
			}
			break;
		case GLFW_KEY_LEFT_SHIFT:
			if(characterController != nullptr)
			{
				characterController->run(action == GLFW_PRESS);
			}
			break;
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

void take_screenshot(const char* filename, int w, int h)
{
	FILE *fp = fopen(filename, "wb");
	if(!fp)
	{
		printf("THERE WAS AN ERROR CREATING THE FILE:%s\n", filename);
		return;
	}	
	
	int nSize = w*h*3;
	char* dataBuffer = (char*)malloc(nSize);

	if(!dataBuffer)
	{
		printf("NO MORE MEMORY!\n");
		fclose(fp);
		return;

	}
	GLuint fbo;
	GLuint renderBuf;
	glGenFramebuffers(1, &fbo);
	glGenRenderbuffers(1, &renderBuf);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_BGR, w, h);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuf);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glReadPixels((GLint)0, (GLint)0, (GLint)w, (GLint)h, GL_BGR, GL_UNSIGNED_BYTE, dataBuffer);

	unsigned char TGAheader[12] = { 0,0,2,0,0,0,0,0,0,0,0,0 };
	unsigned char v0 = static_cast<unsigned char>(w%256);
	unsigned char v1 = static_cast<unsigned char>(w/256);
	unsigned char v2 = static_cast<unsigned char>(h%256);
	unsigned char v3 = static_cast<unsigned char>(h/256);
	unsigned char header[6] = 
	{
		v0, v1, v2, v3, 24, 0 
	};

	fwrite(TGAheader, sizeof(unsigned char), 12, fp);
	fwrite(header, sizeof(unsigned char), 6, fp);
	fwrite(dataBuffer, sizeof(GLubyte), nSize, fp);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Return to onscreen rendering
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &renderBuf);

	free(dataBuffer);
	fclose(fp);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	const char *cname = "click";
	take_screenshot(cname, wWidth, wHeight);
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

// TODO reopen when Piping the JSON through stdin 
	// char ch;
	// while(read(STDIN_FILENO, &ch, 1) > 0)
	// {
	// 	printf("%c", ch);
	// }

	const char *json = "{\"file_id\":\"shot-166-dev\",\"animations\":[1,2,3],\"export_type\":\"screenshot\",\"width\":300,\"height\":300,\"aspect_ratio\":1,\"frame_start\":15,\"frame_end\":30,\"fps\":30,\"camera_panning\":{\"x\":0,\"y\":0},\"zoom\":50}";
	rapidjson::Document d;
	d.Parse(json);

	// PrettyPrint parsed result
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	d.Accept(writer);
	printf("JUST GOT:\n%s\n", sb.GetString());
	
	nima::TGAWriter* photographer = new nima::TGAWriter(d);

	glfwSetErrorCallback(error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	nima::Vec2D m_ViewCenter(3.0f, 30.0f);
	float m_CameraScale = 0.3f;
	nima::Mat2D viewTransform;
	nima::Mat2D inverseViewTransform;

	if (d.HasMember("width") && d["width"].IsInt())
	{
		wWidth = d["width"].GetInt();
		printf("GOT WIDTH:%d\n", wWidth);
	}

	if (d.HasMember("height") && d["height"].IsInt())
	{
		wHeight = d["height"].GetInt();
		printf("GOT HEIGHT:%d\n", wHeight);
	}

	GLFWwindow* window = glfwCreateWindow(wWidth, wHeight, "Nima", NULL, NULL);
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
		actor->load("Assets/SlidingSolo.nima");
		// actor->load(fileName);
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

	nima::GameActorInstance* actorInstance = actor->instance<nima::GameActorInstance>();
	actorInstance->initializeGraphics(renderer);

	nima::ActorAnimation* animation = actorInstance->animation("Slide");
	
	if(d.HasMember("fps") && d["fps"].IsInt())
	{
		int configFps = d["fps"].GetInt();
		animation->fps(configFps);
		printf("ANIMATION FRAMES:%d\n", animation->fps());
	}
	float animationTime = 0.0f;
	if(d.HasMember("frame_start") && d["frame_start"].IsInt())
	{
		int fs = d["frame_start"].GetInt();
		animationTime = (float)fs / (float)animation->fps();
		printf("ANIMATION WILL START FROM:%f\n", animationTime);
	}

	if(d.HasMember("frame_end") && d["frame_end"].IsInt())
	{
		float dur = d["frame_end"].GetInt() / animation->fps();
		animation->duration(dur);
		printf("ANIMATION WILL END AT :%f\n", dur);
	}

	int imgSeqIndex = 0;

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

			m_ViewCenter[1] = height;

			viewTransform[0] = m_CameraScale;
			viewTransform[3] = m_CameraScale;
			viewTransform[4] = (-m_ViewCenter[0] * m_CameraScale + width/2.0f);
			viewTransform[5] = 230;//(-m_ViewCenter[1] * m_CameraScale + height/2.0f);
			renderer->setView(viewTransform);

			nima::Mat2D::invert(inverseViewTransform, viewTransform);
		}

		nima::Vec2D worldMouse(screenMouse[0], lastScreenHeight - screenMouse[1]);
		nima::Vec2D::transform(worldMouse, worldMouse, inverseViewTransform);

		double time = glfwGetTime();
		float elapsed = (float)(time - lastTime);
		lastTime = time;

		if(animation != nullptr)
		{
			if(animationTime + elapsed > animation->duration())
			{
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
			}
			animationTime = std::fmod(animationTime + elapsed, animation->duration());
			animation->apply(animationTime, actorInstance, 1.0f);

			actorInstance->advance(elapsed);
			renderer->clear();
			actorInstance->render(renderer);

			char fname[1024];
			snprintf(fname, sizeof(fname), "screenshot%d.tga", imgSeqIndex++);
			// printf("GOING FOR SCREENSHOT:%s\n", fname);
			photographer->screenshot(fname);
			// take_screenshot(fname, wWidth, wHeight);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup.
	delete actorInstance;
	delete actor;
	delete renderer;
	delete photographer;

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
