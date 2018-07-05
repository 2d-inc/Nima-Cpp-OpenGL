#ifndef _NIMA_GLRENDERER2D_HPP_
#define _NIMA_GLRENDERER2D_HPP_

#include "../Renderer2D.hpp"
#include "GLShaderProgram.hpp"

namespace nima
{
	class GLShader;
	class GLShaderProgram;
	class GLShaderAttribute;
	class GLVertexBuffer;
	class GraphicsBuffer;

	class GLRenderer2D : public Renderer2D
	{
		private:
			BlendMode m_BlendMode;
			int m_ViewportWidth;
			int m_ViewportHeight;
			float m_ProjectionMatrix[16];
			float m_ViewMatrix[16];
			float m_TransformMatrix[16];
			bool m_ViewDirty;
			Color m_Color;
			int m_MaxAttributes;
			unsigned char* m_EnabledAttributes;
			unsigned char* m_WantedAttributes;
			GLShaderResources m_Shaders;

			Texture* m_LastTexture;
			GraphicsBuffer* m_LastBaseBuffer;
			GraphicsBuffer* m_LastPositionBuffer;
			GraphicsBuffer* m_LastUVBuffer;
			int m_LastUVOffset;
			GLShaderProgram* m_LastShader;

			GLShaderProgram m_RegularShader;
			GLShaderProgram m_DeformingShader;

			void enableAttribute(int index);
			void disableAttribute(int index);
			
		  public:
			GLRenderer2D();
			~GLRenderer2D();

			BlendMode blendMode() const override;
			void setBlendMode(BlendMode mode) override;
			void setViewportSize(int width, int height) override;
			void clear() override;
			void setView(const Mat2D& view) override;
			Texture* makeTexture(const Bitmap* bitmap, int flags) override;
			Texture* makeTexture(const std::string& filename, int flags) override;
			GraphicsBuffer* makeVertexBuffer() override;
			GraphicsBuffer* makeIndexBuffer() override;

			void prep(Texture *texture, const Color &color, float opacity, const Mat2D &transform, GraphicsBuffer *vertexBuffer, const float *boneMatrices, int boneMatricesLength, GraphicsBuffer *deformBuffer, GraphicsBuffer *sequenceUVBuffer, int UVoffset) override;
			void draw(const GraphicsBuffer *indexBuffer, int indexCount, int offset) override;
	};
}

#endif