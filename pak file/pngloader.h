#pragma once
#include "bitmap.h"

namespace image_loader
{
	lg::Log<std::fstream> Log()
	{
		boost::shared_ptr<std::fstream> pLogFile(new std::fstream("log.txt",std::ios::out|std::ios::app));
		return lg::Log<std::fstream>(pLogFile,"PNG Loader");
	}
	// object for loading png files
	template <typename Stream>
	class PNGLoad : public bmp::Filter
	{
	public:
		// loads the png file at path_ into bmap
		PNGLoad(Stream &fs_,bmp::Bitmap &bmap) : fs(fs_) { bmap.ApplyFilter(*this); };
		// loads the png file at path into bmap
		void operator()(bmp::RawData &raw);
	private:
		Stream &fs; // reference to stream
	};
	// object for saving png files
	template <typename Stream>
	class PNGSave : public bmp::Filter
	{
	public:
		// loads the png file at path_ into bmap
		PNGSave(Stream &fs_,bmp::Bitmap &bmap) : fs(fs_) { bmap.ApplyFilter(*this); };
		// loads the png file at path into bmap
		void operator()(bmp::RawData &raw);
	private:
		Stream &fs; // reference to stream
	};

	namespace util // implimentation specific utilities
	{
		// returns the next row pointer with each call to operator()
		class RowCounter
		{
		public:
			RowCounter(bmp::PixelBuffer_t &pixels,uint32 x_) : pos((png_bytep)&*pixels.begin()),x(x_) {};
			png_bytep operator()() {
				png_bytep temp=pos;
				pos+=x;
				return temp;
			};
			png_bytep	pos; // current position
			uint32		x; // increment value
		};

		// input stream read function
		template <typename Stream>
		void IFStreamRead(png_structp png,png_bytep data, png_size_t length)
		{
			Stream *ifs = (Stream *)png_get_io_ptr(png);
			ifs->read((char *)data,static_cast<std::streamsize>(length));
		}

		// output stream write function
		template <typename Stream>
		void OFStreamWrite(png_structp png,png_bytep data, png_size_t length)
		{
			Stream *ofs = (Stream *)png_get_io_ptr(png);
			ofs->write((char *)data,static_cast<std::streamsize>(length));
		}

		// output stream flush function
		template <typename Stream>
		void OFStreamFlush(png_structp png)
		{
			Stream *ofs = (Stream *)png_get_io_ptr(png);
			ofs->flush();
		}

		// applies RAII to png pointers
		class PNGPtrs
		{
		public:
			PNGPtrs() : png(0),info(0)
			{
				png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
				if (!png)
					throw ex::out_of_memory(Log(),"png_create_read_struct failed");
				info = png_create_info_struct(png);
				if (!info)
				{
					png_free_data(png, info,PNG_FREE_ALL,-1);
					throw ex::out_of_memory(Log(),"png_create_read_struct failed");
				}
			};
			~PNGPtrs() { png_free_data(png, info,PNG_FREE_ALL,-1); };

			png_infop info;
			png_structp png;
		};

		// sets the row pointers vector
		void SetRows(std::vector<png_bytep> &rows,bmp::RawData &raw)
		{
			rows.resize(raw.Height());
			RowCounter counter(raw.pixels, raw.Width()*raw.BytesPerPixel());
			std::generate(rows.begin(),rows.end(),counter);
		}
	}

	template <typename Stream>
	void PNGLoad<Stream>::operator()(bmp::RawData &raw)
	{
		// initialize png structs
		util::PNGPtrs pngptrs;

		const png_size_t HEADERSIZE = 8;
		char header[HEADERSIZE];	// 8 is the maximum size that can be checked

		fs.read(header, HEADERSIZE);
		if (png_sig_cmp((png_bytep)header, 0, HEADERSIZE))
			throw ex::unrecognized_file_format(Log(),"PNG file expected.");

		if (setjmp(png_jmpbuf(pngptrs.png)))
			throw ex::logic_error(Log(),"Initializing i/o failed.");

		png_set_read_fn(pngptrs.png,&fs,util::IFStreamRead<Stream>);
		png_set_sig_bytes(pngptrs.png, HEADERSIZE);
		png_read_info(pngptrs.png, pngptrs.info);

		raw.description.width =  pngptrs.info->width;
		raw.description.height =  pngptrs.info->height;
		png_byte colorType =  pngptrs.info->color_type;
		png_byte bitDepth =  pngptrs.info->bit_depth;
		// set the bit map color type using the png color info
		switch (colorType)
		{
		case PNG_COLOR_TYPE_GRAY:
			switch (bitDepth)
			{
			case 8:
				raw.description.color.reset(new bmp::GRAY8());
				break;
			case 16:
				raw.description.color.reset(new bmp::GRAY16());
				break;
			default:
				throw ex::value_out_of_bounds(Log(),"Only bit the depths 8 and 16 are supported.");
			};
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			switch (bitDepth)
			{
			case 8:
				raw.description.color.reset(new bmp::BA8());
				break;
			case 16:
				raw.description.color.reset(new bmp::BA16());
				break;
			default:
				throw ex::value_out_of_bounds(Log(),"Only bit the depths 8 and 16 are supported.");
			};
			break;
		case PNG_COLOR_TYPE_RGB:
			switch (bitDepth)
			{
			case 8:
				raw.description.color.reset(new bmp::RGB8());
				break;
			case 16:
				raw.description.color.reset(new bmp::RGB16());
				break;
			default:
				throw ex::value_out_of_bounds(Log(),"Only bit the depths 8 and 16 are supported.");
			};
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			switch (bitDepth)
			{
			case 8:
				raw.description.color.reset(new bmp::RGBA8());
				break;
			case 16:
				raw.description.color.reset(new bmp::RGBA16());
				break;
			default:
				throw ex::value_out_of_bounds(Log(),"Only bit the depths 8 and 16 are supported.");
			};
			break;
		default:
			throw ex::value_out_of_bounds(Log(),"Invalid color format.");
		}

		png_read_update_info(pngptrs.png,  pngptrs.info);


		// read file
		if (setjmp(png_jmpbuf(pngptrs.png)))
			throw ex::read_error(Log(),"Error reading image.");

		raw.pixels.resize(raw.Size());
		std::vector<png_bytep> rows;
		util::SetRows(rows,raw);

		png_read_image(pngptrs.png,(png_bytep *)&*rows.begin());

		// end read
		if (setjmp(png_jmpbuf(pngptrs.png)))
			throw ex::write_error(Log(),"Error reading end of image.");

		png_read_end(pngptrs.png,pngptrs.info);
	}

	template <typename Stream>
	void PNGSave<Stream>::operator()(bmp::RawData &raw)
	{
		// initialize png structs
		util::PNGPtrs pngptrs;

		if (setjmp(png_jmpbuf(pngptrs.png)))
			throw ex::logic_error(Log(),"Initializing i/o failed.");

		png_set_write_fn(pngptrs.png,fs,util::OFStreamWrite<Stream>,util::OFStreamFlush<Stream>);

		// write header
		if (setjmp(png_jmpbuf(pngptrs.png)))
			throw ex::write_error(Log(),"Error writting image header.");

		// get color type
		png_byte colorType;
		switch (raw.NChannels())
		{
		case 1:
			colorType = PNG_COLOR_TYPE_GRAY;
			break;
		case 2:
			colorType = PNG_COLOR_TYPE_GRAY_ALPHA;
			break;
		case 3:
			colorType = PNG_COLOR_TYPE_RGB;
			break;
		case 4:
			colorType = PNG_COLOR_TYPE_RGB_ALPHA;
			break;
		default:
			throw ex::value_out_of_bounds(Log(),"Invalid color format.");
		}

		png_set_IHDR(pngptrs.png, pngptrs.info, raw.Width(), raw.Height(),
				raw.BytesPerChannel()*8, colorType, PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		png_write_info(pngptrs.png, pngptrs.info);


		// write bytes
		if (setjmp(png_jmpbuf(pngptrs.png)))
			throw ex::write_error(Log(),"Error writting image.");
		
		std::vector<png_bytep> rows;
		util::SetRows(rows,raw);

		png_write_image(pngptrs.png,(png_bytep *)&*rows.begin());


		// end write
		if (setjmp(png_jmpbuf(pngptrs.png)))
			throw ex::write_error(Log(),"Error writting end of image.");

		png_write_end(pngptrs.png, NULL);
	}
};
