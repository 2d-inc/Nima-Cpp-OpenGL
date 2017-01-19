#include "GLTexture.hpp"
#include <GLFW/glfw3.h>

using namespace nima;

GLTexture::GLTexture()
{
	glGenTextures(1, &m_Id);
}

GLTexture::~GLTexture()
{
	glDeleteTextures(1, &m_Id);
}

unsigned int GLTexture::id() const
{
	return m_Id;
}