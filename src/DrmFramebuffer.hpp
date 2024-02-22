#ifndef ADS_DRM_FRAMEBUFFER_H
#define ADS_DRM_FRAMEBUFFER_H

#include <cstdint>

#include "Framebuffer.hpp"

namespace adapsurf {

	/**
	 * A framebuffer that all surfaces are rendered to.
	 * @note Currently this must have a stride size (number of bytes between rows) that is compatible with Cairo.
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

			/** Driver supplied framebuffer id. */
			uint32_t _fbId;

			/** Whether the framebuffer was allocated. */
			bool _fbAlloc;

			/** The address where the framebuffer is mapped to */
			void* _fbMapAddr;

			/** Deallocate any resources held by this. */
			void __dealloc();
	};
}

#endif