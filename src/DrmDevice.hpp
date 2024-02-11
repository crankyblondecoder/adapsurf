#ifndef ADS_DRM_DEVICE_H
#define ADS_DRM_DEVICE_H

#include <string>

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

			/**
			 * @param cardNumber Device file card number. This is appended to "/dev/dri/card"
			 * @param connectorIndex Index of connector (display) to use. -1 To use the first available connected connector.
			 */
			DrmDevice(unsigned cardNumber, int connectorIndex);

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
			 * @param prefTabNum Number of tabs to prefix the output with.
			 */
			void enumerateResources(unsigned prefTabNum);

		protected:

		private:

			/** DRM file descriptor. */
			int _devFd;

			/** Index in connectors array of the connector to use. */
			int _connectorIndex;

			/**
			 * Full pathname of the dri device file.
			 */
			std::string _driDeviceFilePathName;

			/** This contains "information about the current display configuration". */
			drmModeResPtr _drmResources;

			/** Whether there is dumb buffer support. DRM_CAP_DUMB_BUFFER*/
			bool _dumbBufferSupport;

			/** The reported preferred depth of dumb buffers. DRM_CAP_DUMB_PREFERRED_DEPTH */
			uint64_t _dumbBufferPrefDepth;
	};
}

#endif