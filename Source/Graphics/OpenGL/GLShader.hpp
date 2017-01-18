#ifndef _NIMA_GLSHADER_HPP_
#define _NIMA_GLSHADER_HPP_

#include <nima/Mat2D.hpp>
#include <nima/Vec2D.hpp>
#include <nima/Color.hpp>
#include "../Renderer2D.hpp"
#include <string>

namespace nima
{
	class GLShader
	{
		private:
			unsigned int m_Id;

		public:
			GLShader(const std::string& filename);
			~GLShader();

			unsigned int id() const { return m_Id; }

	};
}

#endif