#ifndef _NIMA_GLSHADERATTRIBUTE_HPP_
#define _NIMA_GLSHADERATTRIBUTE_HPP_

#include <string>

namespace nima
{
	class GLShaderProgram;
	class GLShaderAttribute
	{
		friend class ShaderProgram;

		private:
			std::string m_Name;
			int m_Position;
			unsigned int m_Size;

			GLShaderAttribute(const GLShaderAttribute& attribute, int position);

		public:
			GLShaderAttribute();
			GLShaderAttribute(std::string name);

			const std::string& name() const;
			unsigned int size() const;
			int position() const;
			void position(int p);

			GLShaderAttribute& operator=(const GLShaderAttribute& other);
	};
}

#endif