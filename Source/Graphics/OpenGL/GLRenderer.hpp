#ifndef _NIMA_GLRENDERER_HPP_
#define _NIMA_GLRENDERER_HPP_

#include <nima/Mat2D.hpp>
#include <nima/Vec2D.hpp>
#include <nima/Color.hpp>
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
		protected:
			GLShaderResources m_Shaders;
			const GLShaderProgram* m_BoundShader;
			const GLVertexBuffer* m_BoundVertexBuffer;

		protected:
			GLRenderer();
			void unbind();
			bool bind(const GLShaderProgram* program, const GLVertexBuffer* vertexBuffer);
			inline bool bind(const GLShaderProgram* program, const GraphicsBuffer* vertexBuffer) { return bind(program, reinterpret_cast<const GLVertexBuffer*>(vertexBuffer)); }

			bool bind(const GLShaderProgram* program, const GLVertexBuffer* vertexBuffer, const GLVertexBuffer* secondaryVertexBuffer);
			inline bool bind(const GLShaderProgram* program, const GraphicsBuffer* vertexBuffer, const GraphicsBuffer* secondaryVertexBuffer) { return bind(program, reinterpret_cast<const GLVertexBuffer*>(vertexBuffer), reinterpret_cast<const GLVertexBuffer*>(secondaryVertexBuffer)); }

	};
}

#endif