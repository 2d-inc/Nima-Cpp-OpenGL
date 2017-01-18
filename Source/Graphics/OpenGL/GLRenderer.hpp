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
	class GLVertexBuffer;
	class GraphicsBuffer;

	class GLRenderer
	{
		private:
			GLShaderResources m_Shaders;
			const GLShaderProgram* m_BoundShader;
			const GLVertexBuffer* m_BoundVertexBuffer;

		protected:
			GLRenderer();
			bool bind(const GLShaderProgram* program, const GLVertexBuffer* vertexBuffer);

	};
}

#endif