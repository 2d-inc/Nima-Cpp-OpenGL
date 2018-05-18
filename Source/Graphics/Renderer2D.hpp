#ifndef _NIMA_RENDERER2D_HPP_
#define _NIMA_RENDERER2D_HPP_

#include <nima/Mat2D.hpp>
#include <nima/Vec2D.hpp>
#include <nima/Color.hpp>
#include <nima/BlendMode.hpp>
#include <string>

namespace nima
{
	class GraphicsBuffer;
	class Texture;
	class Bitmap;

	class Renderer2D
	{
		public:
			virtual ~Renderer2D() {}
			virtual BlendMode blendMode() const = 0;
			virtual void setBlendMode(BlendMode mode) = 0;
			virtual void setViewportSize(int width, int height) = 0;
			virtual void clear() = 0;
			virtual void setView(const Mat2D& view) = 0;
			virtual void prep(Texture *texture, const Color &color, float opacity, const Mat2D &transform, GraphicsBuffer *vertexBuffer, const float *boneMatrices, int boneMatricesLength,GraphicsBuffer *deformBuffer, GraphicsBuffer *sequenceUVBuffer, int UVoffset) = 0;
			virtual void draw(const GraphicsBuffer *indexBuffer, int indexCount) = 0;

			virtual Texture* makeTexture(const Bitmap* bitmap, int flags) = 0;
			virtual Texture* makeTexture(const std::string& filename, int flags) = 0;
			virtual GraphicsBuffer* makeVertexBuffer() = 0;
			virtual GraphicsBuffer* makeIndexBuffer() = 0;
	};
}

#endif