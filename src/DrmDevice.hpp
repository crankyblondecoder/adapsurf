#ifndef ADS_DRM_DEVICE_H
#define ADS_DRM_DEVICE_H

#include <string>

// You may need to symlink drm.h and drm_mode.h into /usr/include/ for the following to work.
#include <xf86drm.h>
#include <xf86drmMode.h>

#include "Device.hpp"
#include "DrmFramebuffer.hpp"

namespace adapsurf
{
	/**
	 * Linux DRM based device.
	 */
	class DrmDevice : public Device
	{
		public:

			virtual ~DrmDevice();

			/**
			 * @param cardNumber Device file card number. This is appended to "/dev/dri/card"
			 * @param connectorIndex Index of connector (display) to use. -1 To use the first available connected connector.
			 * @param modeIndex Index of mode that connector publishes to use. -1 To use preferred mode.
			 */
			DrmDevice(unsigned cardNumber, int connectorIndex, int modeIndex);

			/**
			 * Describe the available DRM resources on std out for this device.
			 * @param prefTabNum Number of tabs to prefix the output with.
			 */
			void enumerateResources(unsigned prefTabNum);

			/**
			 * Enumerate resources for the given dri device card number.
			 * @param cardNumber Device file card number to open.
			 * @param prefTabNum Number of tabs to prefix standard output by.
			 */
			static void enumerateDeviceResources(unsigned cardNumber, unsigned prefTabNum);

			/** libDrm page flip even handler. */
			static void pageFlipEventHandler(int fd, unsigned sequence, unsigned tv_sec, unsigned tv_usec, void* userData);

			/**
			 * Do a page flip. ie Swap the ctrc to use the current framebuffer and make another framebuffer current.
			 * If a previous page flip is still pending then this function will block until it is complete or a timeout occurs.
			 */
			void pageFlip();

			/** Clear the back buffer with the given colour. */
			void clear(double red, double green, double blue);

			// Impl.
			uint32_t getDisplayResolutionWidth();

			// Impl.
			uint32_t getDisplayResolutionHeight();

			// Impl.
			Framebuffer* getDrawToFramebuffer();

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

			/**
			 * Pointer to connector mode that is this device will use. Relies on the connector being valid.
			 * This refers to information that is part of the connector information so doesn't need to be freed.
			 */
			drmModeModeInfoPtr _connectorMode;

			/** The ID of the crtc to use to scan out the framebuffers of this device. */
			uint32_t _crtcId;

			/** The crtc state prior to this device taking over. Required to restore state on destruction. */
			drmModeCrtcPtr _prevCrtc;

			/** Whether there is dumb buffer support. DRM_CAP_DUMB_BUFFER*/
			bool _dumbBufferSupport;

			/** The reported preferred depth of dumb buffers. DRM_CAP_DUMB_PREFERRED_DEPTH */
			uint64_t _dumbBufferPrefDepth;

			// Device is double buffered. ie Front/back.

			/** First framebuffer. */
			DrmFramebuffer* _fb1;

			/** Second framebuffer. */
			DrmFramebuffer* _fb2;

			/** Third framebuffer. */
			DrmFramebuffer* _fb3;

			/** The current framebuffer that is bound to the crtc. ie The front buffer, which is being displayed. */
			unsigned _curFbNum;

			/** If true then a page flip has been requested but not completed. */
			bool _pageFlipping;

			/**
			 * Generate a framebuffer for rendering to this device.
			 * @note This version only generates a framebuffer at driver native resolution.
			 */
			DrmFramebuffer* __generateFramebuffer();

			/** Deallocate any resources held by this. */
			void __dealloc();

			/** Get the current back buffer. */
			DrmFramebuffer* __getBackBuffer();
	};
}

#endif