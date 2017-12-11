#include "GLIndexBuffer.hpp"
#ifdef GL_NEEDS_GLAD
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

using namespace nima;


GLIndexBuffer::GLIndexBuffer() : 
	m_Id(0),
	m_Size(0)
{
	glGenBuffers(1, &m_Id);
}

GLIndexBuffer::~GLIndexBuffer()
{
	glDeleteBuffers(1, &m_Id);
}

void GLIndexBuffer::setData(void* data, int size, BufferHint hint)
{
	m_Size = size;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, hint == BufferHint::Static ? GL_STATIC_DRAW : GL_STREAM_DRAW);
}

void GLIndexBuffer::bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
}

int GLIndexBuffer::size() const
{
	return m_Size;
}

unsigned int GLIndexBuffer::id() const
{
	return m_Id;
}