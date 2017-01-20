#ifndef _NIMA_GLRENDERER2D_HPP_
#define _NIMA_GLRENDERER2D_HPP_

#include "../Renderer2D.hpp"
#include "GLRenderer.hpp"
#include "GLShaderProgram.hpp"

namespace nima
{
	class GLShader;
	class GLShaderProgram;
	class GLShaderAttribute;
	class GLVertexBuffer;
	class GraphicsBuffer;

	class GLRenderer2D : public GLRenderer, public Renderer2D
	{
		private:
			BlendMode m_BlendMode;
			int m_ViewportWidth;
			int m_ViewportHeight;
			float m_ProjectionMatrix[16];
			float m_ViewMatrix[16];
			float m_TransformMatrix[16];

			GLShaderProgram m_TexturedShader;
			GLShaderProgram m_DeformedTexturedShader;
			GLShaderProgram m_TexturedSkinShader;
			GLShaderProgram m_DeformedTexturedSkinShader;

		public:
			GLRenderer2D();

			BlendMode blendMode() const override;
			void setBlendMode(BlendMode mode) override;
			void setViewportSize(int width, int height) override;
			void clear() override;
			void setView(const Mat2D& view) override;
			void drawTextured(const Mat2D& transform, const GraphicsBuffer* vertexBuffer, const GraphicsBuffer* indexBuffer, int offset, int indexCount, float opacity, const Color& color, const Texture* texture) override;
			void drawTexturedAndDeformed(const Mat2D& transform, const GraphicsBuffer* deformBuffer, const GraphicsBuffer* vertexBuffer, const GraphicsBuffer* indexBuffer, int offset, int indexCount, float opacity, const Color& color, const Texture* texture) override;
			void drawTexturedSkin(const Mat2D& transform, const GraphicsBuffer* vertexBuffer, const GraphicsBuffer* indexBuffer, int offset, int indexCount, float* boneMatrices, int boneMatricesLength, float opacity, const Color& color, const Texture* texture) override;
			void drawTexturedAndDeformedSkin(const Mat2D& transform, const GraphicsBuffer* deformBuffer, const GraphicsBuffer* vertexBuffer, const GraphicsBuffer* indexBuffer, int offset, int indexCount, float* boneMatrices, int boneMatricesLength, float opacity, const Color& color, const Texture* texture) override;
			Texture* makeTexture(const Bitmap* bitmap, int flags) override;
			Texture* makeTexture(const std::string& filename, int flags) override;
			GraphicsBuffer* makeVertexBuffer() override;
			GraphicsBuffer* makeIndexBuffer() override;

	};
}

#endif