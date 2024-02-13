#ifndef ADS_FRAMEBUFFER_H
#define ADS_FRAMEBUFFER_H

#include <cstdint>

namespace adapsurf {

	/**
	 * A framebuffer that all surfaces are rendered to.
	 * @note Currently this must have a stride size (number of bytes between rows) that is compatible with Cairo.
	 */
	class Framebuffer
	{
		public:

			~Framebuffer();

			/**
			 * Constructor
			 * @param width The number of pixels wide to allocate.
			 * @param height The number of pixels high to allocate.
			 * @param bpp The number of bits per pixel.
			 * @param stride The number of bytes between two consecutive lines.
			 * @param pixelRatio Ratio of pixel width to height.
			 * @param buffer The allocated buffer.
			 * @param bufferSize The size of the allocated buffer in bytes.
			 */
			Framebuffer(unsigned width, unsigned height, unsigned bpp, unsigned stride, unsigned pixelRatio,
				uint8_t* buffer, unsigned bufferSize);

		protected:

		private:

			/** Width of framebuffer in pixels. */
			unsigned _width;

			/** Height of framebuffer in pixels. */
			unsigned _height;

			/** Number of bits per pixel. */
			unsigned _bpp;

			/** Number of bytes between the start of the rows. */
			unsigned _stride;

			/** The ratio of the width to height (width / height) of the pixels the framebuffer displays. */
			float _pixelRatio;

			/** The actual buffer memory. Not owned by this. */
			uint8_t* _buffer;

			/** Total amount of memory that was allocated to the framebuffer. In bytes. */
			unsigned _bufferSize;
	};
}

#endif