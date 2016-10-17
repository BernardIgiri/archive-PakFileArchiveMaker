#pragma once
#include "bitmap.h"

namespace bmp
{
	namespace filters
	{
		// Inverts the color contents of bmap
		class Invert : public Filter
		{
		public:
			void operator()(bmp::RawData &raw);
		};
		// multiplies color with alpha
		class MultAlpha : public Filter
		{
		public:
			void operator()(bmp::RawData &raw);
		};
	};
}
