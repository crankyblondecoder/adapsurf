#ifndef ADS_DRM_DEVICE_H
#define ADS_DRM_DEVICE_H

#include <libdrm/drm.h>

#include "Device.hpp"
#include "Framebuffer.hpp"

namespace adapsurf {

	/**
	 * Linux DRM based device.
	 */
	class DrmDevice
	{
		public:

			~DrmDevice();
			DrmDevice();

			/**
			 * Generate a framebuffer for rendering to this device.
			 * @param width Width of framebuffer in pixels.
			 * @param height Height of framebuffer in pixels.
			 */
			Framebuffer* generateFramebuffer(unsigned width, unsigned height);

			/**
			 * Destroy a framebuffer generated from this device.
			 * @param fbuf Framebuffer to destroy. Must have been generated from this device.
			 */
			void destroyFramebuffer(Framebuffer* fbuf);

		protected:

		private:

			/** DRM file descriptor. */
			int _devFd;

			/** Full pathname of the dri device file. */
			char* _driDeviceFilePathName;
	};
}

#endif