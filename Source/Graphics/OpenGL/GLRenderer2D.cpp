#include "GLRenderer2D.hpp"
#include "GLIndexBuffer.hpp"
#include "GLVertexBuffer.hpp"
#include "GLTexture.hpp"
#include "../Bitmap/Bitmap.hpp"
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#define BUFFER_OFFSET(i) ((void*)(i))

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
	//glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glDisable(GL_BLEND);

	m_TexturedShader.load(m_Shaders, "Assets/Shaders/Textured.vs", "Assets/Shaders/Textured.fs",
	{
		GLShaderAttribute("VertexPosition", 2, 4, 0),
		GLShaderAttribute("VertexTexCoord", 2, 4, 2)
	},
	{
		GLShaderUniform("ProjectionMatrix"),
		GLShaderUniform("ViewMatrix"),
		GLShaderUniform("WorldMatrix"),
		GLShaderUniform("TextureSampler"),
		GLShaderUniform("Opacity"),
		GLShaderUniform("Color")
	});

	m_DeformedTexturedShader.load(m_Shaders, "Assets/Shaders/Textured.vs", "Assets/Shaders/Textured.fs",
	{
		GLShaderAttribute("VertexPosition", 2, 2, 0)
	},
	{
		GLShaderAttribute("VertexTexCoord", 2, 4, 2)
	},
	{
		GLShaderUniform("ProjectionMatrix"),
		GLShaderUniform("ViewMatrix"),
		GLShaderUniform("WorldMatrix"),
		GLShaderUniform("TextureSampler"),
		GLShaderUniform("Opacity"),
		GLShaderUniform("Color")
	});
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
		case BlendMode::Normal:
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
	printf("VP %i %i\n", width, height);
	glViewport(0, 0, (float)(m_ViewportWidth), (float)(m_ViewportHeight));
	ortho(m_ProjectionMatrix, 0.0f, (float)m_ViewportWidth, 0.0f, (float)m_ViewportHeight, 0.0f, 100.0f);
}

void GLRenderer2D::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
	unbind();
}

void GLRenderer2D::setView(const Mat2D& view)
{
	m_ViewMatrix[0] = view[0];
	m_ViewMatrix[1] = view[1];
	m_ViewMatrix[4] = view[2];
	m_ViewMatrix[5] = view[3];
	m_ViewMatrix[12] = view[4];
	m_ViewMatrix[13] = view[5];
}

void GLRenderer2D::drawTextured(const Mat2D& transform, const GraphicsBuffer* vertexBuffer, const GraphicsBuffer* indexBuffer, int offset, int indexCount, float opacity, const Color& color, const Texture* texture)
{
	m_TransformMatrix[0] = transform[0];
	m_TransformMatrix[1] = transform[1];
	m_TransformMatrix[4] = transform[2];
	m_TransformMatrix[5] = transform[3];
	m_TransformMatrix[12] = transform[4];
	m_TransformMatrix[13] = transform[5];

	if (bind(&m_TexturedShader, vertexBuffer))
	{
		// This value never changes.
		glUniform1i(m_TexturedShader.uniform(3), 0);

		// View and projection transform shouldn't change between re-bind calls.
		glUniformMatrix4fv(m_TexturedShader.uniform(0), 1, GL_FALSE, m_ProjectionMatrix);
		glUniformMatrix4fv(m_TexturedShader.uniform(1), 1, GL_FALSE, m_ViewMatrix);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reinterpret_cast<const GLTexture*>(texture)->id());

	glUniformMatrix4fv(m_TexturedShader.uniform(2), 1, GL_FALSE, m_TransformMatrix);
	glUniform1f(m_TexturedShader.uniform(4), opacity);
	glUniform4fv(m_TexturedShader.uniform(5), 1, color.values());

	const GLIndexBuffer* glIndexBuffer = reinterpret_cast<const GLIndexBuffer*>(indexBuffer);
	glIndexBuffer->bind();
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, BUFFER_OFFSET(sizeof(unsigned short)*offset));

	/*int[] u = m_TexturedShader.Uniforms;
	GL.UniformMatrix4(u[0], true, ref m_Projection);
	GL.UniformMatrix4(u[1], true, ref m_ViewTransform);
	GL.UniformMatrix4(u[2], true, ref m_Transform);
	//GL.UniformMatrix4(u[2], 1, false, xform);

	GL.ActiveTexture(TextureUnit.Texture0);
	GL.BindTexture(TextureTarget.Texture2D, texture.Id);
	GL.Uniform1(u[3], 0);

	GL.Uniform1(u[4], opacity);
	GL.Uniform4(u[5], color);

	GL.BindBuffer(BufferTarget.ElementArrayBuffer, indexBuffer.Id);
	GL.DrawElements(BeginMode.Triangles, indexBuffer.Size, DrawElementsType.UnsignedShort, 0);*/
}

void GLRenderer2D::drawTexturedAndDeformed(const Mat2D& transform, const GraphicsBuffer* deformBuffer, const GraphicsBuffer* vertexBuffer, const GraphicsBuffer* indexBuffer, int offset, int indexCount, float opacity, const Color& color, const Texture* texture)
{
	m_TransformMatrix[0] = transform[0];
	m_TransformMatrix[1] = transform[1];
	m_TransformMatrix[4] = transform[2];
	m_TransformMatrix[5] = transform[3];
	m_TransformMatrix[12] = transform[4];
	m_TransformMatrix[13] = transform[5];

	if (bind(&m_DeformedTexturedShader, deformBuffer, vertexBuffer))
	{
		// This value never changes.
		glUniform1i(m_TexturedShader.uniform(3), 0);
		glUniformMatrix4fv(m_TexturedShader.uniform(1), 1, GL_FALSE, m_ViewMatrix);
		glUniformMatrix4fv(m_TexturedShader.uniform(2), 1, GL_FALSE, m_TransformMatrix);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reinterpret_cast<const GLTexture*>(texture)->id());

	glUniformMatrix4fv(m_TexturedShader.uniform(0), 1, GL_FALSE, m_ProjectionMatrix);
	glUniform1f(m_TexturedShader.uniform(4), opacity);
	glUniform4fv(m_TexturedShader.uniform(5), 1, color.values());

	const GLIndexBuffer* glIndexBuffer = reinterpret_cast<const GLIndexBuffer*>(indexBuffer);
	glIndexBuffer->bind();
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, BUFFER_OFFSET(sizeof(unsigned short)*offset));

	/*int[] u = m_TexturedShader.Uniforms;
	GL.UniformMatrix4(u[0], true, ref m_Projection);
	GL.UniformMatrix4(u[1], true, ref m_ViewTransform);
	GL.UniformMatrix4(u[2], true, ref m_Transform);
	//GL.UniformMatrix4(u[2], 1, false, xform);

	GL.ActiveTexture(TextureUnit.Texture0);
	GL.BindTexture(TextureTarget.Texture2D, texture.Id);
	GL.Uniform1(u[3], 0);

	GL.Uniform1(u[4], opacity);
	GL.Uniform4(u[5], color);

	GL.BindBuffer(BufferTarget.ElementArrayBuffer, indexBuffer.Id);
	GL.DrawElements(BeginMode.Triangles, indexBuffer.Size, DrawElementsType.UnsignedShort, 0);*/
}

Texture* GLRenderer2D::makeTexture(const std::string& filename, int flags)
{
	Bitmap bmp;
	bmp.loadFromPNG(filename, true);
	return makeTexture(&bmp, flags);
}

Texture* GLRenderer2D::makeTexture(const Bitmap* bitmap, int flags)
{
	GLTexture* texture = new GLTexture();
	glBindTexture(GL_TEXTURE_2D, texture->id());

	GLint format;
	switch (bitmap->numChannels())
	{
		case 4:
			format = GL_RGBA;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 2:
			format = GL_LUMINANCE_ALPHA;
			break;
		case 1:
			format = GL_LUMINANCE;
			break;
		default:
			format = GL_RGB;
			break;
	}

	unsigned int width = bitmap->width();
	unsigned int height = bitmap->height();

	bool isPowerOf2 = (width != 0) && ((width & (width - 1)) == 0) && (height != 0) && ((height & (height - 1)) == 0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D,
	             0,
	             format,
	             width, height,
	             0,
	             format,
	             GL_UNSIGNED_BYTE,
	             bitmap->pixels());

	if (!isPowerOf2 || (flags & Texture::ClampToEdge) != 0x00)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, isPowerOf2 ? GL_LINEAR : GL_NEAREST);
	if (flags & Texture::MipMap)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	}

	return texture;
}

GraphicsBuffer* GLRenderer2D::makeVertexBuffer()
{
	return new GLVertexBuffer();
}

GraphicsBuffer* GLRenderer2D::makeIndexBuffer()
{
	return new GLIndexBuffer();
}