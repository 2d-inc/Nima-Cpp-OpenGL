#include "GLRenderer.hpp"
#include "GLShaderProgram.hpp"
#include "GLVertexBuffer.hpp"

using namespace nima;

GLRenderer::GLRenderer() :
	m_BoundShader(nullptr),
	m_BoundVertexBuffer(nullptr)
{

}

bool GLRenderer::bind(const GLShaderProgram* program, const GLVertexBuffer* vertexBuffer)
{
	if(m_BoundShader == program && m_BoundVertexBuffer == vertexBuffer)
	{
		return false;
	}

	m_BoundVertexBuffer = vertexBuffer;

	if(m_BoundShader != program)
	{
		if(m_BoundShader != nullptr)
		{
			m_BoundShader->unbind();
		}
		m_BoundShader = program;
	}

	program->bind(vertexBuffer);

	return true;
}

bool GLRenderer::bind(const GLShaderProgram* program, const GLVertexBuffer* vertexBuffer, const GLVertexBuffer* secondaryVertexBuffer)
{
	// There's an assumption made that the primary vertex buffer is never bound without the secondary, when used.
	if(m_BoundShader == program && m_BoundVertexBuffer == vertexBuffer)
	{
		return false;
	}

	m_BoundVertexBuffer = vertexBuffer;

	if(m_BoundShader != program)
	{
		if(m_BoundShader != nullptr)
		{
			m_BoundShader->unbind();
		}
		m_BoundShader = program;
	}

	program->bind(vertexBuffer, secondaryVertexBuffer);

	return true;
}