#ifndef ADS_DRM_DEVICE_H
#define ADS_DRM_DEVICE_H

// You may need to symlink drm.h and drm_mode.h into /usr/include/ for the following to work.
#include <xf86drm.h>
#include <xf86drmMode.h>

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

			/**
			 * Describe the available DRM resources on std out.
			 */
			void enumerateResources();

		protected:

		private:

			/** DRM file descriptor. */
			int _devFd;

			/**
			 * Full pathname of the dri device file.
			 * TODO This shouldn't be hard wired.
			 */
			const char* _driDeviceFilePathName = "/dev/dri/card1";

			/** This contains "information about the current display configuration". */
			drmModeResPtr _drmResources;
	};
}

#endif