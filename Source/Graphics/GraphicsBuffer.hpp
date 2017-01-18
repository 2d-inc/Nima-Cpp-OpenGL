#ifndef _NIMA_GRAPHICSBUFFER_HPP_
#define _NIMA_GRAPHICSBUFFER_HPP_


namespace nima
{
	enum class BufferHint
	{
		Static,
		Dynamic
	};

	class GraphicsBuffer
	{
		public:
			virtual ~GraphicsBuffer() {};
			virtual void setData(void* data, int size, BufferHint hint) = 0;
	};
}

#endif