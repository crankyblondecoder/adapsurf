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

			virtual ~DrmDevice();

			/**
			 * @param cardNumber Device file card number. This is appended to "/dev/dri/card"
			 * @param connectorIndex Index of connector (display) to use. -1 To use the first available connected connector.
			 */
			DrmDevice(unsigned cardNumber, int connectorIndex);

			/**
			 * Describe the available DRM resources on std out.
			 * @param prefTabNum Number of tabs to prefix the output with.
			 */
			void enumerateResources(unsigned prefTabNum);

		protected:

		private:

			/** Full pathname of the dri device file. */
			std::string _driDeviceFilePathName;

			/** DRM file descriptor. */
			int _deviceFd;
			bool _deviceFdAlloc;

			/** This contains information about the current display configuration". */
			drmModeResPtr _drmResources;
			bool _drmResourcesAlloc;

			/** Pointer to connector information for the connector this device interfaces to. */
			drmModeConnectorPtr _connector;
			bool _connectorAlloc;

			/** Pointer to connector mode that is this device will use. Relies on the connector being valid. */
			drmModeModeInfoPtr _connectorMode;

			/** Whether there is dumb buffer support. DRM_CAP_DUMB_BUFFER*/
			bool _dumbBufferSupport;

			/** The reported preferred depth of dumb buffers. DRM_CAP_DUMB_PREFERRED_DEPTH */
			uint64_t _dumbBufferPrefDepth;

			/**
			 * Generate a framebuffer for rendering to this device.
			 * @note This version only generates a framebuffer at driver native resolution.
			 */
			Framebuffer* __generateFramebuffer();

			/** Deallocate any resources held by this. */
			void __dealloc();
	};
}

#endif