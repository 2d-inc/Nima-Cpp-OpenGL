#include "GLRenderer2D.hpp"
#include "GLIndexBuffer.hpp"
#include "GLVertexBuffer.hpp"
#include "GLTexture.hpp"
#include "../Bitmap/Bitmap.hpp"
#ifdef GL_NEEDS_GLAD
#include <glad/glad.h>
#endif
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
	m_TransformMatrix{1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f },
	m_ViewDirty(false),
	m_Color{},
	m_LastTexture(nullptr),
	m_LastBaseBuffer(nullptr),
	m_LastPositionBuffer(nullptr),
	m_LastUVBuffer(nullptr),
	m_LastUVOffset(0),
	m_LastShader(nullptr)
{
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &m_MaxAttributes);
	m_WantedAttributes = new unsigned char[m_MaxAttributes];
	m_EnabledAttributes = new unsigned char[m_MaxAttributes];
	std::memset(m_WantedAttributes, 0, m_MaxAttributes);
	std::memset(m_EnabledAttributes, 0, m_MaxAttributes);
	
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glDisable(GL_BLEND);

	m_RegularShader.load(m_Shaders, "Assets/Shaders/Regular.vs", "Assets/Shaders/Textured.fs",
	{
		GLShaderAttribute("VertexPosition"),
		GLShaderAttribute("VertexTexCoord")
	},
	{
		GLShaderUniform("ProjectionMatrix"),
		GLShaderUniform("ViewMatrix"),
		GLShaderUniform("WorldMatrix"),
		GLShaderUniform("TextureSampler"),
		GLShaderUniform("Color")
	});

	m_DeformingShader.load(m_Shaders, "Assets/Shaders/Deforming.vs", "Assets/Shaders/Textured.fs",
	{
		GLShaderAttribute("VertexPosition"),
		GLShaderAttribute("VertexTexCoord"),
		GLShaderAttribute("VertexBoneIndices"),
		GLShaderAttribute("VertexWeights"),
	},
	{
		GLShaderUniform("ProjectionMatrix"),
		GLShaderUniform("ViewMatrix"),
		GLShaderUniform("WorldMatrix"),
		GLShaderUniform("TextureSampler"),
		GLShaderUniform("Color"),
		GLShaderUniform("BoneMatrices")
	});
}

GLRenderer2D::~GLRenderer2D()
{
	delete [] m_WantedAttributes;
	delete [] m_EnabledAttributes;
}

BlendMode GLRenderer2D::blendMode() const
{
	return m_BlendMode;
}

void GLRenderer2D::setBlendMode(BlendMode mode)
{
	if(m_BlendMode == mode)
	{
		return;
	}
	m_BlendMode = mode;
	switch (m_BlendMode)
	{
		case BlendMode::Off:
			glDisable(GL_BLEND);
			break;
		case BlendMode::Normal:
			glEnable(GL_BLEND);
			//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE);
			 //_GL.blendFuncSeparate(_GL.SRC_ALPHA, _GL.ONE_MINUS_SRC_ALPHA, _GL.SRC_ALPHA, _GL.ONE);
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
	// unbind();
}

void GLRenderer2D::setView(const Mat2D& view)
{
	if( m_ViewMatrix[0] != view[0] &&
		m_ViewMatrix[1] != view[1] &&
		m_ViewMatrix[4] != view[2] &&
		m_ViewMatrix[5] != view[3] &&
		m_ViewMatrix[12] != view[4] &&
		m_ViewMatrix[13] != view[5] )
	{
		return;
	}

	m_ViewDirty = true;
	m_ViewMatrix[0] = view[0];
	m_ViewMatrix[1] = view[1];
	m_ViewMatrix[4] = view[2];
	m_ViewMatrix[5] = view[3];
	m_ViewMatrix[12] = view[4];
	m_ViewMatrix[13] = view[5];
}

Texture* GLRenderer2D::makeTexture(const std::string& filename, int flags)
{
	Bitmap bmp;
	bmp.loadFromPNG(filename, false);
	bmp.multiplyAlpha();
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

void GLRenderer2D::prep(Texture* texture, const Color &color, float opacity, const Mat2D& transform, GraphicsBuffer* vertexBuffer, const float* boneMatrices, int boneMatricesLength, GraphicsBuffer* deformBuffer, GraphicsBuffer* sequenceUVBuffer, int uvOffset)
{
	GLShaderProgram* shader = boneMatricesLength > 0 ? &m_DeformingShader : &m_RegularShader;
	bool changedShader = false;
	if(shader != m_LastShader)
	{
		changedShader = true;
		shader->use();
		m_LastShader = shader;
	}

	if(changedShader || m_LastBaseBuffer != vertexBuffer || m_LastPositionBuffer != deformBuffer || m_LastUVBuffer != sequenceUVBuffer)
	{
		m_LastBaseBuffer = vertexBuffer;
		m_LastPositionBuffer = deformBuffer;
		m_LastUVBuffer = sequenceUVBuffer;
		m_LastUVOffset = uvOffset;

		// We use the base buffer if:
		//	* We're deforming with bones
		// 		- base buffer contains the Bone Indices and Weights so it's necessary no matter what
		// 	* There's no custom position buffer
		//		- means we're not doing an animated vertex deform (user manually moved vertices).
		// 	* There's no custom uv buffer
		// 		- means we're not animating texture coordinates (usually done by the image sequencer)
		bool useBaseBuffer = boneMatricesLength > 0 || deformBuffer == nullptr || sequenceUVBuffer == nullptr;
		if(useBaseBuffer)
		{
			reinterpret_cast<const GLVertexBuffer*>(vertexBuffer)->bind();
			int bufferStride = boneMatricesLength > 0 ? 48 : 16;
			if(deformBuffer == nullptr)
			{
				// position comes from base buffer.
				GLShaderAttribute vertexAttribute = shader->attribute(0); // VertexPosition
				int index = vertexAttribute.position();
				enableAttribute(index);
				glVertexAttribPointer(index, 2, GL_FLOAT, false, bufferStride, 0);
			}
			if(sequenceUVBuffer == nullptr)
			{
				// uv comes from base buffer.
				GLShaderAttribute attr = shader->attribute(1); // VertexTexCoord
				int index = attr.position();
				enableAttribute(index);
				glVertexAttribPointer(index, 2, GL_FLOAT, false, bufferStride, BUFFER_OFFSET(8));
			}
			if(boneMatricesLength > 0)
			{
				GLShaderAttribute boneAttr = shader->attribute(2); // VertexBoneIndices
				int index = boneAttr.position();
				enableAttribute(index);
				glVertexAttribPointer(index, 4, GL_FLOAT, false, 48, BUFFER_OFFSET(16));

				GLShaderAttribute weightsAttr = shader->attribute(3); // VertexWeights
				index = weightsAttr.position();
				enableAttribute(index);
				glVertexAttribPointer(index, 4, GL_FLOAT, false, 48, BUFFER_OFFSET(32));
			}
		}

		if(deformBuffer != nullptr)
		{
			// Using a custom position buffer.
			reinterpret_cast<const GLVertexBuffer*>(deformBuffer)->bind();
			GLShaderAttribute attr = shader->attribute(0); // VertexPosition
			int index = attr.position();
			enableAttribute(index);
			glVertexAttribPointer(index, 2, GL_FLOAT, false, 8, 0);
		}
		if(sequenceUVBuffer != nullptr)
		{
			// Using a custom uv buffer.
			reinterpret_cast<const GLVertexBuffer*>(sequenceUVBuffer)->bind();
			GLShaderAttribute attr = shader->attribute(1); // VertexTexCoord
			int index = attr.position();
			enableAttribute(index);
			glVertexAttribPointer(index, 2, GL_FLOAT, false, 8, BUFFER_OFFSET(uvOffset));
		}

		// Disable unwanted attributes.
		for(int i = 0; i < m_MaxAttributes; i++)
		{
			if(m_WantedAttributes[i] != m_EnabledAttributes[i])
			{
				glDisableVertexAttribArray(i);
				m_EnabledAttributes[i] = 0;
			}
			m_WantedAttributes[i] = 0;
		}
	}
	else if (sequenceUVBuffer != nullptr && m_LastUVOffset != uvOffset)
	{
		m_LastUVOffset = uvOffset;
		reinterpret_cast<const GLVertexBuffer*>(sequenceUVBuffer)->bind();
		GLShaderAttribute attr = shader->attribute(1); // VertexTexCoord
		glVertexAttribPointer(attr.position(), 2, GL_FLOAT, false, 8, BUFFER_OFFSET(uvOffset));
	}

	if(changedShader || m_ViewDirty)
	{
		glUniformMatrix4fv(shader->uniform(0) /* ProjectionMatrix */, 1, GL_FALSE, m_ProjectionMatrix);
		glUniformMatrix4fv(shader->uniform(1) /* ViewMatrix */, 1, GL_FALSE, m_ViewMatrix);
		m_ViewDirty = false;
	}

	m_Color.red(color[0] * opacity);
	m_Color.green(color[1] * opacity);
	m_Color.blue(color[2] * opacity);
	m_Color.alpha(color[3] * opacity);
	glUniform4fv(shader->uniform(4) /* Color */, 1, m_Color.values());

	m_TransformMatrix[0] = transform[0];
	m_TransformMatrix[1] = transform[1];
	m_TransformMatrix[4] = transform[2];
	m_TransformMatrix[5] = transform[3];
	m_TransformMatrix[12] = transform[4];
	m_TransformMatrix[13] = transform[5];
	glUniformMatrix4fv(shader->uniform(2) /* WorldMatrix */, 1, GL_FALSE, m_TransformMatrix);

	if(boneMatricesLength > 0)
	{
		glUniform3fv(shader->uniform(5) /* BoneMatrices */, boneMatricesLength / 3, boneMatrices);
	}
	if(m_LastTexture != texture)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, reinterpret_cast<const GLTexture *>(texture)->id());
		m_LastTexture = texture;
	}

}

void GLRenderer2D::draw(const GraphicsBuffer *indexBuffer, int indexCount, int offset)
{
	const GLIndexBuffer* glIndexBuffer = reinterpret_cast<const GLIndexBuffer *>(indexBuffer);
	GLint boundBufferId = 0;
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &boundBufferId);

	if(boundBufferId != glIndexBuffer->id())
	{
		glIndexBuffer->bind();
	}
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, BUFFER_OFFSET(sizeof(unsigned short)*offset));
}

void GLRenderer2D::enableAttribute(int index)
{
	m_WantedAttributes[index] = 1;
	if(m_EnabledAttributes[index] == 0)
	{
		m_EnabledAttributes[index] = 1;
		glEnableVertexAttribArray(index);
	}
}

void GLRenderer2D::disableAttribute(int index)
{
	if(m_EnabledAttributes[index] != 0)
	{
		m_EnabledAttributes[index] = 0;
		glDisableVertexAttribArray(index);
	}
}