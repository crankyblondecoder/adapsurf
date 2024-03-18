#ifndef ADS_DRM_FRAMEBUFFER_H
#define ADS_DRM_FRAMEBUFFER_H

#include <cstdint>

#include "Framebuffer.hpp"

namespace adapsurf {

	/**
	 * A Linux DRM based framebuffer.
	 * @note For now only XR24 pixel format is supported => [31:0] x:R:G:B 8:8:8:8 little endian
	 */
	class DrmFramebuffer : public Framebuffer
	{
		public:

			virtual ~DrmFramebuffer();

			/**
			 * Constructor
			 * @param device Device file descriptor.
			 * @param width The number of pixels wide to allocate.
			 * @param height The number of pixels high to allocate.
			 * @param depth Colour depth, in bits, of each pixel.
			 * @param bpp Total number of bits per pixel. Must be greater or equal to depth.
			 */
			DrmFramebuffer(int deviceFd, unsigned width, unsigned height, unsigned depth, unsigned bpp);

			uint32_t getFramebufferId();

			/** Clear this buffer with the given colour. */
			void clear(double red, double green, double blue);

			/** Get the Fourcc code that describes the pixel format of this framebuffer. */
			std::string getFourcc();

			// Override.
			void compose(Surface& surface);

			/** Peek into the memory of this framebuffer. Exists to help with debug of mmap'd device memory. */
			uint8_t peek(unsigned offset);

		protected:

		private:

			/** DRM device file descriptor. Required for resource cleanup. */
			int _deviceFd;

			/** Dumb buffer handle. */
			uint32_t _dbHandle;

			/** Whether dumb buffer was allocated. */
			bool _dbAlloc;

			/** Width of framebuffer in pixels. */
			unsigned _width;

			/** Height of framebuffer in pixels. */
			unsigned _height;

			/** Colour depth. ie Number of bits that represents colour. */
			unsigned _depth;

			/** Number of bits per pixel. Can be greater or equal to depth. */
			unsigned _bpp;

			/** Stride of buffer. The number of bytes between two consecutive lines. */
			uint32_t _stride;

			/** Total size of buffer in bytes. */
			uint64_t _size;

			/** The pixel format the framebuffer uses. */
			uint32_t _pixel_format;

			/** Driver supplied framebuffer id. */
			uint32_t _fbId;

			/** Whether the framebuffer was allocated. */
			bool _fbAlloc;

			/** The address where the framebuffer is mapped to */
			uint8_t* _fbMapAddr;

			/** Deallocate any resources held by this. */
			void __dealloc();
	};
}

#endif