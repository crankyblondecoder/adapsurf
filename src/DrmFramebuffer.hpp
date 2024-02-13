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

			~DrmFramebuffer();

			/**
			 * Constructor
			 * @param device Device file descriptor.
			 * @param width The number of pixels wide to allocate.
			 * @param height The number of pixels high to allocate.
			 */
			DrmFramebuffer(int deviceFd, unsigned width, unsigned height);

		protected:

		private:

		/** DRM device file descriptor. Required for resource cleanup. */
		int _deviceFd;

		/** Dumb buffer handle. */
		uint32_t _dbHandle;

		/** Whether dumb buffer was allocated. */
		bool _dbAlloc;

		/** Stride of buffer. The number of bytes between two consecutive lines. */
		uint32_t _stride;

		/** Total size of buffer in bytes. */
    	uint64_t _size;

		/** Driver supplied framebuffer id. */
		uint32_t fbId;

		/** Whether the framebuffer was allocated. */
		bool _fbAlloc;
	};
}

#endif