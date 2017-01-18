#ifndef _NIMA_TEXTURE_HPP_
#define _NIMA_TEXTURE_HPP_

namespace nima
{
	class Texture
	{
		public:

			enum Flags
			{
				MipMap = 1 << 0,
				ClampToEdge = 1 << 1
			};
			virtual ~Texture(){}
	};
}

#endif