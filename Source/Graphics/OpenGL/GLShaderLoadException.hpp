#ifndef _NIMA_GLSHADERLOADEXCEPTION_HPP_
#define _NIMA_GLSHADERLOADEXCEPTION_HPP_

#include <exception>
#include <string>

namespace nima
{
	class GLShaderLoadException : public std::exception
	{
		std::string m_Message;
		std::string m_Filename;

		public:
			GLShaderLoadException(const std::string& msg, const std::string& filename) : m_Message(msg), m_Filename(filename)
			{

			}

			const std::string& message() const
			{
				return m_Message;
			}

			const std::string& filename() const
			{
				return m_Filename;
			}
	};
}

#endif