#ifndef ADS_DEVICE_H
#define ADS_DEVICE_H

#include "framebuffer.hpp"

namespace adapsurf {

	/**
	 * A device abstracts a specific display device and provides a framebuffer into which drawing can take place.
	 */
	class device
	{
		public:

			~device();
			device();

			/**
			 * Generate a framebuffer for rendering to this device.
			 * @param width Width of framebuffer in pixels.
			 * @param height Height of framebuffer in pixels.
			 */
			framebuffer* generateFramebuffer(unsigned width, unsigned height);

			/**
			 * Destroy a framebuffer generated from this device.
			 * @param fbuf Framebuffer to destroy. Must have been generated from this device.
			 */
			void destroyFramebuffer(framebuffer* fbuf);

		protected:

		private:
	};
}

#endif