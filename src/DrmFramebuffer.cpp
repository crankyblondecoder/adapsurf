// You may need to symlink drm.h and drm_mode.h into /usr/include/ for the following to work.
#include <string>
#include <sys/mman.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include "DrmFramebuffer.hpp"
#include "Exception.hpp"

using namespace adapsurf;

DrmFramebuffer::~DrmFramebuffer()
{
	__dealloc();
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

	error = drmModeAddFB(deviceFd, width, height, 24, 32, _stride, _dbHandle, &_fbId);

	if(error < 0)
	{
		__dealloc();

		std::string msg("Error while adding dumb framebuffer: ");
		msg += error;
		throw Exception(Exception::Error::DRM_CREATE_FRAME_BUFFER_FAIL, msg);
	}

	_fbAlloc = true;

	// Prepare dumb buffer for memory mapping

	uint64_t offset;

	error = drmModeMapDumbBuffer(deviceFd, _dbHandle, &offset);

	if(error < 0)
	{
		__dealloc();

		std::string msg("Error while preparing dumb framebuffer for memory mapping: ");
		msg += error;
		throw Exception(Exception::Error::DRM_CREATE_FRAME_BUFFER_FAIL, msg);
	}

	_fbMapAddr = mmap(0, _size, PROT_READ | PROT_WRITE, MAP_SHARED, _deviceFd, offset);

	if(_fbMapAddr == MAP_FAILED) {

		__dealloc();

		std::string msg("Error while memory mapping framebuffer: ");
		msg += errno;
		throw Exception(Exception::Error::DRM_CREATE_FRAME_BUFFER_FAIL, msg);
	}
}

void DrmFramebuffer::__dealloc()
{
	if(_fbAlloc)
	{
		drmModeRmFB(_deviceFd, _fbId);
		_fbAlloc = false;
	}

	_fbAlloc = false;

	if(_dbAlloc)
	{
		drmModeDestroyDumbBuffer(_deviceFd, _dbAlloc);
		_dbAlloc = false;
	}
}