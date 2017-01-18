#include "GLRenderer2D.hpp"
#include <GLFW/glfw3.h>

using namespace nima;

GLRenderer2D::GLRenderer2D() :
	m_BlendMode(BlendMode::Off),
	m_ViewportWidth(0),
	m_ViewportHeight(0),
	m_ProjectionMatrix{0},
	m_ViewMatrix{1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f },
	m_TransformMatrix{1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f }
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glDisable(GL_BLEND);
}

BlendMode GLRenderer2D::blendMode() const
{
	return m_BlendMode;
}

void GLRenderer2D::setBlendMode(BlendMode mode)
{
	m_BlendMode = mode;
	switch (m_BlendMode)
	{
		case BlendMode::Off:
			glDisable(GL_BLEND);

			break;
		case BlendMode::Transparent:
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case BlendMode::Additive:
			glEnable(GL_BLEND);
			glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
			break;
		case BlendMode::Multiply:
			glEnable(GL_BLEND);
			glBlendFuncSeparate(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case BlendMode::Screen:
			glEnable(GL_BLEND);
			glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_COLOR, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			break;
	}
}

static void ortho(float* matrix, float left, float right, float bottom, float top, float near, float far)
{
	float lr = 1.0f / (left - right),
	      bt = 1.0f / (bottom - top),
	      nf = 1.0f / (near - far);
	matrix[0] = -2.0f * lr;
	matrix[1] = 0;
	matrix[2] = 0;
	matrix[3] = 0;
	matrix[4] = 0;
	matrix[5] = -2.0f * bt;
	matrix[6] = 0;
	matrix[7] = 0;
	matrix[8] = 0;
	matrix[9] = 0;
	matrix[10] = 2.0f * nf;
	matrix[11] = 0;
	matrix[12] = (left + right) * lr;
	matrix[13] = (top + bottom) * bt;
	matrix[14] = (far + near) * nf;
	matrix[15] = 1.0f;
}

void GLRenderer2D::setViewportSize(int width, int height)
{
	if (m_ViewportWidth == width && m_ViewportHeight == height)
	{
		return;
	}
	m_ViewportWidth = width;
	m_ViewportHeight = height;

	glViewport(0, 0, (float)(m_ViewportWidth), (float)(m_ViewportHeight));
	ortho(m_ProjectionMatrix, 0.0f, (float)m_ViewportWidth, 0.0f, (float)m_ViewportHeight, 0.0f, 100.0f);
}

void GLRenderer2D::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void GLRenderer2D::drawTextured(const Mat2D& view, const Mat2D& transform, const GraphicsBuffer* vertexBuffer, const GraphicsBuffer* indexBuffer, float opacity, const Color& color, const Texture* texture)
{
	m_ViewMatrix[0] = view[0];
	m_ViewMatrix[1] = view[1];
	m_ViewMatrix[4] = view[2];
	m_ViewMatrix[5] = view[3];
	m_ViewMatrix[12] = view[4];
	m_ViewMatrix[13] = view[5];

	m_TransformMatrix[0] = transform[0];
	m_TransformMatrix[1] = transform[1];
	m_TransformMatrix[4] = transform[2];
	m_TransformMatrix[5] = transform[3];
	m_TransformMatrix[12] = transform[4];
	m_TransformMatrix[13] = transform[5];
}

Texture* GLRenderer2D::makeTexture(const Bitmap* bitmap, int flags)
{
	return nullptr;
}

GraphicsBuffer* GLRenderer2D::makeVertexBuffer()
{
	return nullptr;
}

GraphicsBuffer* GLRenderer2D::makeIndexBuffer()
{
	return nullptr;
}