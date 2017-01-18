#include "GLShader.hpp"
#include "GLShaderLoadException.hpp"
#include <GLFW/glfw3.h>

using namespace nima;

GLShader::GLShader(const std::string& f)
{
	const char* filename = f.c_str();
	if ( filename == NULL )
	{
		throw GLShaderLoadException(f, "NULL filename");
	}
	else
	{
		long len = strlen( filename );
		if ( len < 4 )
		{
			throw GLShaderLoadException(f, "Filename is too short");
		}

		int type;
		if ( strcmp( filename + len - 3, ".fs" ) == 0 )
		{
			type = GL_FRAGMENT_SHADER;
		}
		else if ( strcmp( filename + len - 3, ".vs" ) == 0 )
		{
			type = GL_VERTEX_SHADER;
		}
		else
		{
			throw GLShaderLoadException(f, "Could not determine extension type, must be '.fs' or '.vs'");
		}

		std::FILE* fp = std::fopen(filename, "r");

		if ( fp == NULL )
		{
			throw GLShaderLoadException(f, "Could not open file");
		}

		long int current = ftell(fp);
		fseek(fp, 0, SEEK_END);
		long length = ftell(fp);
		fseek(fp, current, SEEK_SET);

		char* buffer = new char[length + 1];
		if ( buffer == NULL )
		{
			std::fclose(fp);
			throw GLShaderLoadException(f, "out of memory while reading");
		}

		std::fread(buffer, length, 1, fp);
		//fp->read( buffer, length, 1 );
		buffer[length] = '\0';
		std::fclose(fp);


		m_Id = glCreateShader(type);

//#ifdef OSX_MAKEFILE
//		std::string shaderData(buffer);
//		shaderData = std::string("#version 120\n") + shaderData;
//		const char* data = shaderData.c_str();
//		glShaderSource(m_Id, 1, (const char**)&data, NULL);
//#else
		glShaderSource(m_Id, 1, (const char**)&buffer, NULL);
//#endif

		delete [] buffer;

		glCompileShader(m_Id);

		int status;

		glGetShaderiv(m_Id, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
			char infobuffer[2048];
			int infobufferlen = 0;
			glGetShaderInfoLog(m_Id, 2047, &infobufferlen, infobuffer);
			infobuffer[infobufferlen] = 0;
			if ( infobufferlen != 0 )
			{
				if ( strstr( infobuffer, "error" ) == NULL )
				{
					printf("Shader::Load - warnings from GLSL while compiling '%s'\n", filename);
					printf("-----------------------------------------------------------------------\n");
					printf("%s", infobuffer);
					printf("\n");
				}
				else
				{
					printf("Shader::Load - errors from GLSL while compiling '%s'\n", filename);
					printf("-----------------------------------------------------------------------\n");
					printf("%s\n", infobuffer);
					printf("\n");
				}
			}

			throw GLShaderLoadException(f, "compiler errors, check log");
		}
	}
}

GLShader::~GLShader()
{
	if (m_Id != 0)
	{
		glDeleteShader( m_Id );
	}
}