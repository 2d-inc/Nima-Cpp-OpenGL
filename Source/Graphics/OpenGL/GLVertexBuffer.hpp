#ifndef _NIMA_GLVERTEXBUFFER_HPP_
#define _NIMA_GLVERTEXBUFFER_HPP_

#include "../GraphicsBuffer.hpp"

namespace nima
{
	class GLVertexBuffer: public GraphicsBuffer
	{
		private:
			unsigned int m_Id;
			int m_Size;

		public:
			GLVertexBuffer();
			virtual ~GLVertexBuffer();
			virtual void setData(void* data, int size, BufferHint hint) override;

			void bind() const;
			int size() const;
			unsigned int id() const;
	};
}

#endif