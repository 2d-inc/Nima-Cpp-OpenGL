#include "GLShaderAttribute.hpp"

using namespace nima;

GLShaderAttribute::GLShaderAttribute() : 
	m_Position(0),
	m_Size(0),
	m_Stride(0),
	m_StrideInBytes(0),
	m_Offset(0),
	m_BufferPosition(nullptr)
{

}

GLShaderAttribute::GLShaderAttribute(const GLShaderAttribute& attribute, int position) :
	m_Name(attribute.m_Name),
	m_Position(position),
	m_Size(attribute.m_Size),
	m_Stride(attribute.m_Stride),
	m_StrideInBytes(attribute.m_StrideInBytes),
	m_Offset(attribute.m_Offset),
	m_BufferPosition(attribute.m_BufferPosition)
{
}

GLShaderAttribute::GLShaderAttribute(std::string name, unsigned int size, unsigned int stride, unsigned int offset) :
	m_Name(name),
	m_Position(-1),
	m_Size(size),
	m_Stride(stride),
	m_StrideInBytes(stride * sizeof(float)),
	m_Offset(offset)
{
	float* buffer = nullptr;
	m_BufferPosition = buffer + offset;
}

GLShaderAttribute& GLShaderAttribute::operator=(const GLShaderAttribute& other)
{
	m_Name = other.m_Name;
	m_Position = other.m_Position;
	m_Size = other.m_Size;
	m_Stride = other.m_Stride;
	m_StrideInBytes = other.m_StrideInBytes;
	m_Offset = other.m_Offset;
	m_BufferPosition = other.m_BufferPosition;

	return *this;
}

const std::string& GLShaderAttribute::name() const
{
	return m_Name;
}

unsigned int GLShaderAttribute::size() const
{
	return m_Size;
}

unsigned int GLShaderAttribute::stride() const
{
	return m_Stride;
}

unsigned int GLShaderAttribute::strideInBytes() const
{
	return m_StrideInBytes;
}

unsigned int GLShaderAttribute::offset() const
{
	return m_Offset;
}

int GLShaderAttribute::position() const
{
	return m_Position;
}

void GLShaderAttribute::position(int p)
{
	m_Position = p;
}

float* GLShaderAttribute::bufferPosition() const
{
	return m_BufferPosition;
}