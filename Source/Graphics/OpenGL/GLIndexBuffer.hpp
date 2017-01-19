#ifndef _NIMA_GLINDEXBUFFER_HPP_
#define _NIMA_GLINDEXBUFFER_HPP_

#include "../GraphicsBuffer.hpp"

namespace nima
{
	class GLIndexBuffer: public GraphicsBuffer
	{
		private:
			unsigned int m_Id;
			int m_Size;

		public:
			GLIndexBuffer();
			~GLIndexBuffer();
			virtual void setData(void* data, int size, BufferHint hint) override;

			void bind() const;
			int size() const;
			unsigned int id() const;
	};
}

#endif