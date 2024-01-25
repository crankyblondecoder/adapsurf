#ifndef ADS_DEVICE_H
#define ADS_DEVICE_H

#include "Framebuffer.hpp"

namespace adapsurf {

	/**
	 * A device abstracts a specific display device and provides a framebuffer into which drawing can take place.
	 */
	class Device
	{
		public:

			~Device(){};
			Device(){};

			/**
			 * Generate a framebuffer for rendering to this device.
			 * @param width Width of framebuffer in pixels.
			 * @param height Height of framebuffer in pixels.
			 */
			virtual Framebuffer* generateFramebuffer(unsigned width, unsigned height) = 0;

			/**
			 * Destroy a framebuffer generated from this device.
			 * @param fbuf Framebuffer to destroy. Must have been generated from this device.
			 */
			virtual void destroyFramebuffer(Framebuffer* fbuf) = 0;

		protected:

		private:
	};
}

#endif