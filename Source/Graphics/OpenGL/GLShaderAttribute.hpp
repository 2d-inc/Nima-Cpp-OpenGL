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
			unsigned int m_Stride;
			unsigned int m_StrideInBytes;
			unsigned int m_Offset;
			float* m_BufferPosition;

			GLShaderAttribute(const GLShaderAttribute& attribute, int position);

		public:
			GLShaderAttribute();
			GLShaderAttribute(std::string name, unsigned int size, unsigned int stride, unsigned int offset);

			const std::string& name() const;
			unsigned int size() const;
			unsigned int stride() const;
			unsigned int strideInBytes() const;
			unsigned int offset() const;
			int position() const;
			void position(int p);
			float* bufferPosition() const;

			GLShaderAttribute& operator=(const GLShaderAttribute& other);
	};
}

#endif