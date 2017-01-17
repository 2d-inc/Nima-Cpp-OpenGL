#include "Bitmap.hpp"
#include "DecodeBitmapException.hpp"
#include "UnknownBitmapFormatException.hpp"
#include <png.h>
#include <cstdio>

using namespace nima;

Bitmap::Bitmap() :
	m_Pixels(nullptr),
	m_Width(0),
	m_Height(0),
	m_NumChannels(0)
{

}

Bitmap::~Bitmap()
{
	delete [] m_Pixels;
}

void Bitmap::load(const std::string& filename, bool flipY)
{
	size_t index = filename.rfind('.');
	if (index == std::string::npos)
	{
		throw new UnknownBitmapFormatException("Bitmap filename has no extension.", filename);
	}

	
	std::string extension(filename, index);
	std::transform(extension.begin(), extension.end(), extension.begin(), std::tolower);
	if (extension == ".png")
	{
		// handle
		loadFromPNG(filename, flipY);
	}
	else
	{
		// Could add jpg, pvrtc, etc.
		throw new UnknownBitmapFormatException("Bitmap format not supported.", filename);
	}
}

void Bitmap::loadFromPNG(const std::string& filename, bool flipY)
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;

	png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr );

	if ( png_ptr == nullptr )
	{
		throw new DecodeBitmapException("libpng failed (png_create_read_struct)", filename);
	}

	info_ptr = png_create_info_struct( png_ptr );
	if ( info_ptr == nullptr )
	{
		png_destroy_read_struct( &png_ptr, (png_infopp)nullptr, (png_infopp)nullptr );
		throw new DecodeBitmapException("libpng failed (png_create_info_struct)", filename);
	}
//#if ANDROID
	if ( setjmp( png_jmpbuf(png_ptr ) ) )
	{
		png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp)nullptr );
		throw new DecodeBitmapException("not a valid png file", filename);
		//LOGI( "Bitmap::LoadPNG - \"%s\" is not a valid png file.", filename == nullptr ? "???" : filename );
		//return false;
	}
//#endif
	//std::string resourcePath = Platform::resourcePath(filename);
	std::FILE* fp = std::fopen(filename.c_str(), "rb");

	png_init_io( png_ptr, fp );
	png_read_info( png_ptr, info_ptr );


	png_get_IHDR( png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, nullptr, nullptr );

	png_set_strip_16( png_ptr );

	m_NumChannels = 0;

	if ( color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_RGB )
	{
		png_set_expand( png_ptr );
		m_NumChannels = 3;
		if ( png_get_valid( png_ptr, info_ptr, PNG_INFO_tRNS ) )
		{
			png_set_expand( png_ptr );
			m_NumChannels++;
		}
	}
	else if ( color_type == PNG_COLOR_TYPE_GRAY )
	{
		png_set_expand( png_ptr );
		m_NumChannels = 1;
	}
	else if ( color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
	{
		png_set_expand( png_ptr );
		png_set_gray_to_rgb( png_ptr );
		m_NumChannels = 4;
	}
	else if ( color_type == PNG_COLOR_TYPE_RGB_ALPHA )
	{
		png_set_expand( png_ptr );
		m_NumChannels = 4;
	}
	png_read_update_info(png_ptr, info_ptr);

	m_Width = (unsigned int)width;
	m_Height = (unsigned int)height;

	m_Pixels = new unsigned char[width * height * m_NumChannels];


	png_uint_32 row_stride = (png_uint_32)png_get_rowbytes(png_ptr, info_ptr);
	png_bytep* row_pointers = new png_bytep[height];

	if ( setjmp( png_jmpbuf(png_ptr) ) )
	{
		png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp)nullptr );
		throw new DecodeBitmapException("not a valid png file during read", filename);
	}

	if ( row_stride != width * m_NumChannels )
	{
		unsigned char *t = new unsigned char[height * row_stride];
		for (unsigned row = 0; row < height; row++)
		{
			unsigned int rIndex = row;
			if (flipY)
			{
				rIndex = height - row - 1;
			}
			row_pointers[rIndex] = t + (row * row_stride);
		}

		png_read_image( png_ptr, row_pointers );
		png_read_end( png_ptr, info_ptr );

		for ( unsigned int i = 0; i < height; i++ )
		{
			for ( unsigned int c = 0; c < width; c++ )
			{
				m_Pixels[(i * width + c)*m_NumChannels] = row_pointers[i][c * m_NumChannels];
				if ( m_NumChannels == 1 )
				{
					continue;
				}
				m_Pixels[(i * width + c)*m_NumChannels + 1] = row_pointers[i][c * m_NumChannels + 1];
				m_Pixels[(i * width + c)*m_NumChannels + 2] = row_pointers[i][c * m_NumChannels + 2];
				if ( m_NumChannels == 3 )
				{
					continue;
				}
				m_Pixels[(i * width + c)*m_NumChannels + 3] = row_pointers[i][c * m_NumChannels + 3];
			}
		}

		delete [] t;
	}
	else
	{
		for ( unsigned row = 0; row < height; row++ )
		{
			unsigned int rIndex = row;
			if (flipY)
			{
				rIndex = height - row - 1;
			}

			row_pointers[rIndex] = m_Pixels + (row * row_stride);
		}
		png_read_image( png_ptr, row_pointers );
		png_read_end( png_ptr, info_ptr );
	}

	png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp)nullptr );

	delete [] row_pointers;
	std::fclose(fp);
}
