#ifndef _NIMA_BITMAP_HPP_
#define _NIMA_BITMAP_HPP_

#include <string>

namespace nima
{
	class Bitmap
	{
		private:
			unsigned char* m_Pixels;
			unsigned int m_Width;
			unsigned int m_Height;
			unsigned int m_NumChannels;

		public:
			Bitmap();
			~Bitmap();

			void loadFromPNG(const std::string& filename, bool flipY = false);
			void load(const std::string& filename, bool flipY = false);

			unsigned int numChannels() const;
			unsigned int width() const;
			unsigned int height() const;
			unsigned char* pixels() const;

			void multiplyAlpha();
	};
}

#endif