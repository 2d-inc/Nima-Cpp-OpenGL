#ifndef _NIMA_GLSHADERPROGRAM_HPP_
#define _NIMA_GLSHADERPROGRAM_HPP_

#include "GLShader.hpp"
#include "GLShaderAttribute.hpp"
#include "GLShaderResources.hpp"
#include <vector>

namespace nima
{
	typedef std::string GLShaderUniform;

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
			void bind();
			void unbind();

			int uniform(int idx) const { return m_Uniforms[idx]; }
	};
}

#endif