#ifndef _NIMA_RENDERER2D_HPP_
#define _NIMA_RENDERER2D_HPP_

#include <nima/Mat2D.hpp>
#include <nima/Vec2D.hpp>
#include <nima/Color.hpp>

namespace nima
{
	class GraphicsBuffer;
	class Texture;
	class Bitmap;

	enum class BlendMode
	{
		Off = 0,
		Transparent,
		Multiply,
		Screen,
		Additive
	};

	class Renderer2D
	{
		public:
			virtual BlendMode blendMode() const = 0;
			virtual void setBlendMode(BlendMode mode) = 0;
			virtual void resize(int width, int height) = 0;
			virtual void drawTextured(const Mat2D& view, const Mat2D& transform, const GraphicsBuffer* vertexBuffer, const GraphicsBuffer* indexBuffer, float opacity, const Color& color, const Texture* texture) = 0;

			virtual Texture* makeTexture(const Bitmap* bitmap, int flags) = 0;
			virtual GraphicsBuffer* makeVertexBuffer() = 0;
			virtual GraphicsBuffer* makeIndexBuffer() = 0;
	};
}

#endif