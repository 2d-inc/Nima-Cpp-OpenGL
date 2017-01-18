#include "GLVertexBuffer.hpp"
#include <GLFW/glfw3.h>

using namespace nima;

GLVertexBuffer::GLVertexBuffer() :
	m_Id(0),
	m_Size(0)
{
	glGenBuffers(1, &m_Id);
}

GLVertexBuffer::~GLVertexBuffer()
{
	glDeleteBuffers(1, &m_Id);
}

void GLVertexBuffer::setData(void* data, int size, BufferHint hint)
{
	m_Size = size;
	glBindBuffer(GL_ARRAY_BUFFER, m_Id);
	glBufferData(GL_ARRAY_BUFFER, size, data, hint == BufferHint::Static ? GL_STATIC_DRAW : GL_STREAM_DRAW);
}

void GLVertexBuffer::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Id);
}

int GLVertexBuffer::size() const
{
	return m_Size;
}

unsigned int GLVertexBuffer::id() const
{
	return m_Id;
}