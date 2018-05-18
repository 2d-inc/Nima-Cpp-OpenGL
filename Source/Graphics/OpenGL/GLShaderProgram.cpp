#include "GLShaderProgram.hpp"
#include "GLVertexBuffer.hpp"
#ifdef GL_NEEDS_GLAD
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

using namespace nima;

GLShaderProgram::GLShaderProgram() :
	m_Id(0),
	m_VertexShader(nullptr),
	m_FragmentShader(nullptr),
	m_Attributes(0),
	m_AttributeCount(0),
	m_Uniforms(0),
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

	delete[] m_Uniforms;
	m_Uniforms = nullptr;
	m_UniformCount = 0;

	delete [] m_Attributes;
	m_Attributes = nullptr;
	m_AttributeCount = 0;
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

	idx = 0;
	m_UniformCount = uniforms.size();
	m_Uniforms = new int[m_UniformCount];
	int texSamplerLocation = -1;
	int secondTexSamplerLocation = -1;

	for (auto u : uniforms)
	{
		int l = glGetUniformLocation(m_Id, u.c_str());
		if (l == -1)
		{
			if(u == "SecondTextureSampler")
			{
				// Don't warn of an optional sampler (might want to make this more robust in the future).
				continue;
			}
			printf("ShaderProgram::load - couln't find uniform %s (%s | %s).\n", u.c_str(), vsFilename.c_str(), fsFilename.c_str());
		}
		else if (u == "TextureSampler")
		{
			texSamplerLocation = l;
		}
		else if(u == "SecondTextureSampler")
		{
			secondTexSamplerLocation = l;
		}
		m_Uniforms[idx++] = l;
	}
	
	// We always use texture unit 0 for our sampler. Set it once.
	glUniform1i(texSamplerLocation, 0);
	if(secondTexSamplerLocation != -1)
	{
		glUniform1i(secondTexSamplerLocation, 1);
	}

	return true;
}

void GLShaderProgram::use()
{
	glUseProgram(m_Id);
}