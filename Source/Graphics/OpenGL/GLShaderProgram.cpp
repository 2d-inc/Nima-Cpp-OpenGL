#include "GLShaderProgram.hpp"
#include <GLFW/glfw3.h>

using namespace nima;

GLShaderProgram::GLShaderProgram() :
	m_Id(0),
	m_VertexShader(nullptr),
	m_FragmentShader(nullptr),
	m_Attributes(nullptr),
	m_AttributeCount(0),
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
	unload();

	m_FragmentShader = shaderResources.get(fsFilename);
    m_VertexShader = shaderResources.get(vsFilename);
    
    if(m_FragmentShader == nullptr || m_VertexShader == nullptr)
    {
        return false;
    }
    
    m_Id = glCreateProgram();
    glAttachShader(m_Id, m_VertexShader->id());
    glAttachShader(m_Id, m_FragmentShader->id());
    glLinkProgram(m_Id);
    glUseProgram(m_Id);

    m_Attributes = new GLShaderAttribute[attributes.size()];
    int idx = 0;
    for(auto a : attributes)
    {
        int location = glGetAttribLocation(m_Id, a.name().c_str());
        if(location == -1)
        {
            printf("ShaderProgram::load - couln't find attribute %s (%s | %s).\n", a.name().c_str(), vsFilename.c_str(), fsFilename.c_str());
        }
        a.position(location);
        m_Attributes[idx++] = a;
    }

    idx = 0;
    m_Uniforms = new int[uniforms.size()];
    for(auto u : uniforms)
    {
        int l = glGetUniformLocation(m_Id, u.c_str());
        if(l == -1)
        {
            printf("ShaderProgram::load - couln't find uniform %s (%s | %s).\n", u.c_str(), vsFilename.c_str(), fsFilename.c_str());
        }
        m_Uniforms[idx++] = l;
    }


	return true;
}

void GLShaderProgram::bind()
{

}

void GLShaderProgram::unbind()
{

}