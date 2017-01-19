#ifndef _NIMA_GLTEXTURE_HPP_
#define _NIMA_GLTEXTURE_HPP_

#include "../Texture.hpp"

namespace nima
{
	class GLTexture : public Texture
	{
		private:
			unsigned int m_Id;

		public:
			GLTexture();
			~GLTexture();
			unsigned int id() const;
	};
}

#endif