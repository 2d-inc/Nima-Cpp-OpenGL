#ifndef _NIMA_GLRENDERER_HPP_
#define _NIMA_GLRENDERER_HPP_

#include <nima/Mat2D.hpp>
#include <nima/Vec2D.hpp>
#include <nima/Color.hpp>
#include "../Renderer2D.hpp"
#include "GLShaderResources.hpp"

namespace nima
{
	class GLShader;
	class GLShaderProgram;
	class GLShaderAttribute;

	class GLRenderer
	{
		private:
			GLShaderResources m_Shaders;
			
		protected:
	};
}

#endif