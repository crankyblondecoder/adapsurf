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

DrmFramebuffer::DrmFramebuffer(int deviceFd, unsigned width, unsigned height, unsigned depth, unsigned bpp)
	: _deviceFd{deviceFd}
{
	_width = width;
	_height = height;
	_depth = depth;
	_bpp = bpp;

	// Try and allocate dumb buffer.
	_dbAlloc = false;

	int error = drmModeCreateDumbBuffer(deviceFd, width, height, bpp, 0, &_dbHandle, &_stride, &_size);

	if(error < 0)
	{
		std::string msg("Error during dumb buffer creation: ");
		msg += std::to_string(error);
		throw Exception(Exception::Error::DRM_CREATE_FRAME_BUFFER_FAIL, msg);
	}

	_dbAlloc = true;

	// Try and allocate the framebuffer.
	_fbAlloc = false;

	error = drmModeAddFB(deviceFd, width, height, depth, bpp, _stride, _dbHandle, &_fbId);

	if(error < 0)
	{
		__dealloc();

		std::string msg("Error while adding dumb framebuffer: ");
		msg += std::to_string(error);
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
		msg += std::to_string(error);
		throw Exception(Exception::Error::DRM_CREATE_FRAME_BUFFER_FAIL, msg);
	}

	_fbMapAddr = mmap(0, _size, PROT_READ | PROT_WRITE, MAP_SHARED, _deviceFd, offset);

	if(_fbMapAddr == MAP_FAILED) {

		__dealloc();

		std::string msg("Error while memory mapping framebuffer: ");
		msg += std::to_string(errno);
		throw Exception(Exception::Error::DRM_CREATE_FRAME_BUFFER_FAIL, msg);
	}

	// If gets to here then the framebuffer has been successfully setup.
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

uint32_t DrmFramebuffer::getFramebufferId()
{
	return _fbId;
}

void DrmFramebuffer::clear(double red, double green, double blue)
{
	// TODO ...
}

std::string DrmFramebuffer::getFourcc()
{
	std::string retString;

	if(_fbAlloc)
	{
		drmModeFB2Ptr fbInfo = drmModeGetFB2(_deviceFd, _fbId);

		uint32_t fourcc_char1 = (fbInfo -> pixel_format) | 0xF;
		uint32_t fourcc_char2 = (fbInfo -> pixel_format >> 8) | 0xF;
		uint32_t fourcc_char3 = (fbInfo -> pixel_format >> 16) | 0xF;
		uint32_t fourcc_char4 = (fbInfo -> pixel_format >> 24) | 0xF;

		retString += (char) fourcc_char1;
		retString += (char) fourcc_char2;
		retString += (char) fourcc_char3;
		retString += (char) fourcc_char4;

		drmModeFreeFB2(fbInfo);
	}

	return retString;
}