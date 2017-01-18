#include "GLShaderResources.hpp"
#include "GLShader.hpp"

using namespace nima;

GLShaderResources::~GLShaderResources()
{
	for (auto it : m_LoadedShaders)
	{
		delete it.second;
	}
}

GLShader* GLShaderResources::get(std::string name)
{
	auto it = m_LoadedShaders.find(name);
	if (it != m_LoadedShaders.end())
	{
		return it->second;
	}
	else
	{
		GLShader* resource = new GLShader(name);
		m_LoadedShaders[name] = resource; 
		return resource;
	}
}