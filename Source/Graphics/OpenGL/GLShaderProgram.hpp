#ifndef _NIMA_GLSHADERPROGRAM_HPP_
#define _NIMA_GLSHADERPROGRAM_HPP_

#include "GLShader.hpp"
#include "GLShaderAttribute.hpp"
#include "GLShaderResources.hpp"
#include <vector>

namespace nima
{
	typedef std::string GLShaderUniform;
	class GLVertexBuffer;
	class GLShaderProgram
	{
		private:
			unsigned int m_Id;
			GLShader* m_VertexShader;
			GLShader* m_FragmentShader;

			GLShaderAttribute* m_Attributes;
			int m_AttributeCount;

			int* m_Uniforms;
			int m_UniformCount;

			void unload();

		public:
			GLShaderProgram();
			~GLShaderProgram();


			bool load(GLShaderResources& shaderResources, const std::string& vsFilename, const std::string& fsFilename, std::vector<GLShaderAttribute> attributes, std::vector<GLShaderUniform> uniforms);
			bool load(GLShaderResources& shaderResources, const std::string& vsFilename, const std::string& fsFilename, std::vector<GLShaderAttribute> attributes, std::vector<GLShaderAttribute> secondaryAttributes, std::vector<GLShaderUniform> uniforms);
			void use();			

			int uniform(int index) { return m_Uniforms[index]; }
			GLShaderAttribute attribute(int index) { return m_Attributes[index]; }
	};
}

#endif