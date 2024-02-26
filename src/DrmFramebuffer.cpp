// You may need to symlink drm.h, drm_mode.h and drm_fourcc.h into /usr/include/ for the following to work.
#include <drm_fourcc.h>
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
	: _deviceFd{deviceFd}, _pixel_format{0}
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

	// Make sure the expected pixel format is present.

	drmModeFB2Ptr fbInfo = drmModeGetFB2(_deviceFd, _fbId);

	if(fbInfo)
	{
		_pixel_format = fbInfo -> pixel_format;
		drmModeFreeFB2(fbInfo);
	}

	if(_pixel_format != DRM_FORMAT_XRGB8888) {

		__dealloc();

		std::string msg("The DRM Framebuffer currently only supports the XR24 pixel format.");
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
	// Assume XR24 pixel format only.

	uint8_t clRed = red * 255;
	uint8_t clGreen = green * 255;
	uint8_t clBlue = blue * 255;

	uint8_t* buf = (uint8_t*) _fbMapAddr;
	uint8_t* bufEnd = buf + _size;

	while(buf < bufEnd)
	{
		// Byte 0 is blue.
		*buf++ = clBlue;
		// Byte 1 is green.
		*buf++ = clGreen;
		// Byte 2 is red.
		*buf++ = clRed;
		// Byte 3 is not used.
		buf++;
	}
}

std::string DrmFramebuffer::getFourcc()
{
	/*
	 * Typically this will be XR24: [31:0] x:R:G:B 8:8:8:8 little endian
	 *
	 *     Byte 0: B
	 *     Byte 1: G
	 *     Byte 2: R
	 *     Byte 3: X (not used)
	 */

	std::string retString;

	if(_fbAlloc)
	{
		drmModeFB2Ptr fbInfo = drmModeGetFB2(_deviceFd, _fbId);

		if(fbInfo)
		{
			uint32_t fourcc_char1 = (fbInfo -> pixel_format) & 0xFF;
			uint32_t fourcc_char2 = (fbInfo -> pixel_format >> 8) & 0xFF;
			uint32_t fourcc_char3 = (fbInfo -> pixel_format >> 16) & 0xFF;
			uint32_t fourcc_char4 = (fbInfo -> pixel_format >> 24) & 0xFF;

			retString += (char) fourcc_char1;
			retString += (char) fourcc_char2;
			retString += (char) fourcc_char3;
			retString += (char) fourcc_char4;

			drmModeFreeFB2(fbInfo);
		}
	}

	return retString;
}

void DrmFramebuffer::compose(Surface& surface)
{
	uint8_t* pixelData = surface.getPixelData();

	if(pixelData)
	{
		unsigned size = surface.getPixelDataSize();
		unsigned stride = surface.getPixelDataStride();

		// TODO ... Short circuit the composition if none of the target surface is within this framebuffer.

		// TODO ... Copy to current to buffer while taking into account the pixel data has pre-multiplied alpha.
	}
}