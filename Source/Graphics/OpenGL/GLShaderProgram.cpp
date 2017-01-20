#include "GLShaderProgram.hpp"
#include "GLVertexBuffer.hpp"
#include <GLFW/glfw3.h>

using namespace nima;

GLShaderProgram::GLShaderProgram() :
	m_Id(0),
	m_VertexShader(nullptr),
	m_FragmentShader(nullptr),
	m_Attributes(nullptr),
	m_AttributeCount(0),
	m_SecondaryAttributes(nullptr),
	m_SecondaryAttributeCount(0),
	m_Uniforms(nullptr),
	m_UniformCount(0)
{
}

GLShaderProgram::~GLShaderProgram()
{
	unload();
}

void GLShaderProgram::unload()
{
	if (m_Id != 0)
	{
		if (m_VertexShader)
		{
			glDetachShader(m_Id, m_VertexShader->id());
		}
		if (m_FragmentShader)
		{
			glDetachShader(m_Id, m_FragmentShader->id());
		}

		glDeleteProgram(m_Id);
		m_Id = 0;
		m_VertexShader = nullptr;
		m_FragmentShader = nullptr;
	}

	delete[] m_Attributes;
	delete[] m_Uniforms;

	m_Attributes = nullptr;
	m_Uniforms = nullptr;
	m_AttributeCount = 0;
	m_UniformCount = 0;
}

bool GLShaderProgram::load(GLShaderResources& shaderResources, const std::string& vsFilename, const std::string& fsFilename, std::vector<GLShaderAttribute> attributes, std::vector<GLShaderUniform> uniforms)
{
	return load(shaderResources, vsFilename, fsFilename, attributes, {}, uniforms);
}

bool GLShaderProgram::load(GLShaderResources& shaderResources, const std::string& vsFilename, const std::string& fsFilename, std::vector<GLShaderAttribute> attributes, std::vector<GLShaderAttribute> secondaryAttributes, std::vector<GLShaderUniform> uniforms)
{
	unload();

	m_FragmentShader = shaderResources.get(fsFilename);
	m_VertexShader = shaderResources.get(vsFilename);

	if (m_FragmentShader == nullptr || m_VertexShader == nullptr)
	{
		return false;
	}

	m_Id = glCreateProgram();
	glAttachShader(m_Id, m_VertexShader->id());
	glAttachShader(m_Id, m_FragmentShader->id());
	glLinkProgram(m_Id);
	glUseProgram(m_Id);

	m_AttributeCount = attributes.size();
	m_Attributes = new GLShaderAttribute[m_AttributeCount];
	int idx = 0;
	for (auto a : attributes)
	{
		int location = glGetAttribLocation(m_Id, a.name().c_str());
		if (location == -1)
		{
			printf("ShaderProgram::load - couln't find attribute %s (%s | %s).\n", a.name().c_str(), vsFilename.c_str(), fsFilename.c_str());
		}
		a.position(location);
		m_Attributes[idx++] = a;
	}

	m_SecondaryAttributeCount = secondaryAttributes.size();
	if(m_SecondaryAttributeCount > 0)
	{
		m_SecondaryAttributes = new GLShaderAttribute[m_SecondaryAttributeCount];
		int idx = 0;
		for (auto a : secondaryAttributes)
		{
			int location = glGetAttribLocation(m_Id, a.name().c_str());
			if (location == -1)
			{
				printf("ShaderProgram::load - couln't find secondary attribute %s (%s | %s).\n", a.name().c_str(), vsFilename.c_str(), fsFilename.c_str());
			}
			a.position(location);
			m_SecondaryAttributes[idx++] = a;
		}
	}

	idx = 0;
	m_UniformCount = uniforms.size();
	m_Uniforms = new int[m_UniformCount];

	for (auto u : uniforms)
	{
		int l = glGetUniformLocation(m_Id, u.c_str());
		if (l == -1)
		{
			printf("ShaderProgram::load - couln't find uniform %s (%s | %s).\n", u.c_str(), vsFilename.c_str(), fsFilename.c_str());
		}
		m_Uniforms[idx++] = l;
	}

	return true;
}

void GLShaderProgram::bind(const GLVertexBuffer* vertexBuffer) const
{
	glUseProgram(m_Id);
	vertexBuffer->bind();
	for(int i = 0; i < m_AttributeCount; i++)
	{
		const GLShaderAttribute& attribute = m_Attributes[i];
		if(attribute.position() == -1)
		{
			continue;
		}
		glEnableVertexAttribArray(attribute.position());
		glVertexAttribPointer(attribute.position(), attribute.size(), GL_FLOAT, GL_FALSE, attribute.strideInBytes(), attribute.bufferPosition());
	}
}

void GLShaderProgram::bind(const GLVertexBuffer* vertexBuffer, const GLVertexBuffer* secondaryVertexBuffer) const
{
	glUseProgram(m_Id);

	vertexBuffer->bind();
	for(int i = 0; i < m_AttributeCount; i++)
	{
		const GLShaderAttribute& attribute = m_Attributes[i];
		if(attribute.position() == -1)
		{
			continue;
		}
		glEnableVertexAttribArray(attribute.position());
		glVertexAttribPointer(attribute.position(), attribute.size(), GL_FLOAT, GL_FALSE, attribute.strideInBytes(), attribute.bufferPosition());
	}

	secondaryVertexBuffer->bind();
	for(int i = 0; i < m_SecondaryAttributeCount; i++)
	{
		const GLShaderAttribute& attribute = m_SecondaryAttributes[i];
		if(attribute.position() == -1)
		{
			continue;
		}
		glEnableVertexAttribArray(attribute.position());
		glVertexAttribPointer(attribute.position(), attribute.size(), GL_FLOAT, GL_FALSE, attribute.strideInBytes(), attribute.bufferPosition());
	}
}

void GLShaderProgram::unbind() const
{
	for(int i = 0; i < m_AttributeCount; i++)
	{
		const GLShaderAttribute& attribute = m_Attributes[i];
		if(attribute.position() == -1)
		{
			continue;
		}
		glDisableVertexAttribArray(attribute.position());
	}
	for(int i = 0; i < m_SecondaryAttributeCount; i++)
	{
		const GLShaderAttribute& attribute = m_SecondaryAttributes[i];
		if(attribute.position() == -1)
		{
			continue;
		}
		glDisableVertexAttribArray(attribute.position());
	}
}