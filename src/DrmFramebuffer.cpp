// You may need to symlink drm.h and drm_mode.h into /usr/include/ for the following to work.
#include <string>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include "DrmFramebuffer.hpp"
#include "Exception.hpp"

using namespace adapsurf;

DrmFramebuffer::~DrmFramebuffer()
{
	// TODO ... Free dumb buffer and frame buffer.

	if(_dbAlloc) drmModeDestroyDumbBuffer(_deviceFd, _dbAlloc);
}

DrmFramebuffer::DrmFramebuffer(int deviceFd, unsigned width, unsigned height) : _deviceFd{deviceFd}
{
	// Try and allocate dumb buffer.
	_dbAlloc = false;

	int error = drmModeCreateDumbBuffer(deviceFd, width, height, 32, 0, &_dbHandle, &_stride, &_size);

	if(error < 0)
	{
		std::string msg("Error during dumb buffer creation: ");
		msg += error;
		throw Exception(Exception::Error::DRM_CREATE_FRAME_BUFFER_FAIL, msg);
	}

	_dbAlloc = true;

	// Try and allocate the framebuffer.
	_fbAlloc = false;

	error = drmModeAddFB(deviceFd, width, height, 24, 32, _stride, _dbHandle, &fbId);

	if(error < 0)
	{
		drmModeDestroyDumbBuffer(deviceFd, _dbAlloc);

		_dbAlloc = false;

		std::string msg("Error while adding dumb framebuffer: ");
		msg += error;
		throw Exception(Exception::Error::DRM_CREATE_FRAME_BUFFER_FAIL, msg);
	}

	_fbAlloc = true;

	// TODO ... Prepare dumb buffer for memory mapping
}