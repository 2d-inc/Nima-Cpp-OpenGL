#include "GLShaderAttribute.hpp"

using namespace nima;

GLShaderAttribute::GLShaderAttribute() : 
	m_Position(0),
	m_Size(0)
{

}
GLShaderAttribute::GLShaderAttribute(const GLShaderAttribute& attribute, int position) :
	m_Name(attribute.m_Name),
	m_Position(position),
	m_Size(attribute.m_Size)
{
}

GLShaderAttribute::GLShaderAttribute(std::string name) :
	m_Name(name),
	m_Position(0),
	m_Size(0)
{}

GLShaderAttribute& GLShaderAttribute::operator=(const GLShaderAttribute& other)
{
	m_Name = other.m_Name;
	m_Position = other.m_Position;
	m_Size = other.m_Size;
	return *this;
}

const std::string& GLShaderAttribute::name() const
{
	return m_Name;
}

int GLShaderAttribute::position() const
{
	return m_Position;
}

void GLShaderAttribute::position(int p)
{
	m_Position = p;
}