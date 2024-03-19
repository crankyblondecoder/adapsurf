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

	_fbMapAddr = (uint8_t*) mmap(0, _size, PROT_READ | PROT_WRITE, MAP_SHARED, _deviceFd, offset);

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
	// TODO Posibly offer an option to create an intermediate buffer to compose into in case the bus between CPU and GPU on
	// small form factor systems (like Pi Zero) is slow.

	int srcPosnX = surface.getGlobalPositionX();
	int srcPosnY = surface.getGlobalPositionY();

	unsigned srcWidth = surface.getWidth();
	unsigned srcHeight = surface.getHeight();

	// Source left and bottom extremities.
	unsigned srcEndX = srcPosnX + srcWidth - 1;
	unsigned srcEndY = srcPosnY + srcHeight - 1;

	// Short circuit the composition if none of the target surface is within this framebuffer.
	if(srcPosnX >= (int)_width || srcPosnY >= (int)_height || srcEndX < 0 || srcEndY < 0) return;

	uint8_t* pixelData = surface.getPixelData();

	if(pixelData)
	{
		//    |---|-------------|---|

		unsigned srcStride = surface.getPixelDataStride();

		// Source line index to start copying from.
		unsigned srcStartLine = srcPosnY < 0 ? -srcPosnY : 0;

		// Source line index to finish copying from.
		unsigned srcEndLine = srcHeight - 1;
		if(srcEndY >= _height) srcEndLine -= srcEndY - _height + 1;

		// Number of source lines to be copied.
		unsigned numLines = srcEndLine - srcStartLine + 1;

		// Start offset in source line to copy from. In pixels.
		unsigned srcLineStartOffset = srcPosnX < 0 ? -srcPosnX : 0;

		// End offset (inclusive) of source line to copy from. In pixels.
		unsigned srcLineEndOffset = srcWidth - 1;
		if(srcEndX >= _width) srcLineEndOffset -= srcEndX - _width + 1;

		// Number of pixels to copy from the source per line.
		unsigned numPixCopyPerLine = srcLineEndOffset - srcLineStartOffset + 1;

		// Number of source bytes to go from end of line to start of next line (with offset).
		unsigned nextSrcLineStride = srcStride - numPixCopyPerLine * 4;

		// Number of fb bytes to go from end of line to start of next line (with offset).
		unsigned nextFbLineStride = _stride - numPixCopyPerLine * 4;

		// Calculate initial source copy from position.
		uint8_t* srcCpPosn = pixelData + srcStartLine * srcStride + srcLineStartOffset * 4;

		// Calculate initial fb copy to position.
		uint8_t* fbCpPosn = _fbMapAddr + (srcPosnY + srcStartLine) * _stride + (srcPosnX + srcLineStartOffset) * 4;

		// Copy to current buffer while taking into account the pixel data has pre-multiplied alpha.

		// Current source pixel values.
		uint8_t srcBlue;
		uint8_t srcGreen;
		uint8_t srcRed;
		uint8_t srcAlpha;

		// Effective alpha value used to scale current fb pixel channels.
		double effAlpha;

		for(unsigned line = 0; line < numLines; line++)
		{
			for(unsigned pixel = 0; pixel < numPixCopyPerLine; pixel++)
			{
				// Work at byte level because it then becomes machine endien agnostic.
				srcBlue = *srcCpPosn++;
				srcGreen = *srcCpPosn++;
				srcRed = *srcCpPosn++;
				srcAlpha = *srcCpPosn++;

				// No point in processing pixel if it is completely transparent.
				if(srcAlpha > 0)
				{
					if(srcAlpha == 0xFF)
					{
						// Just straight copy is required.

						*fbCpPosn = srcBlue;
						fbCpPosn++;
						*fbCpPosn = srcGreen;
						fbCpPosn++;
						*fbCpPosn = srcRed;
						fbCpPosn += 2;
					}
					else
					{
						// Alpha blend.
						effAlpha = 1.0 - ((double)srcAlpha / 255.0);

						*fbCpPosn = srcBlue + (uint8_t)((double)(*fbCpPosn) * effAlpha);
						fbCpPosn++;
						*fbCpPosn = srcGreen + (uint8_t)((double)(*fbCpPosn) * effAlpha);
						fbCpPosn++;
						*fbCpPosn = srcRed + (uint8_t)((double)(*fbCpPosn) * effAlpha);
						fbCpPosn += 2;
					}
				}
				else
				{
					fbCpPosn += 4;
				}
			}

			srcCpPosn += nextSrcLineStride;
			fbCpPosn += nextFbLineStride;
		}
	}
}

uint8_t DrmFramebuffer::peek(unsigned offset)
{
	return *(_fbMapAddr + offset);
}
