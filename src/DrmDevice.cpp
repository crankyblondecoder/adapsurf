#include <fcntl.h>
#include <string>
#include <unistd.h>

#include "DrmDevice.hpp"
#include "Exception.hpp"

using namespace adapsurf;

DrmDevice::~DrmDevice()
{
	if(_devFd >= 0) close(_devFd);
}

DrmDevice::DrmDevice()
{
	_devFd = open(_driDeviceFilePathName, O_RDWR);

	if(_devFd < 0)
	{
		std::string msg("Could not open dri device: ");
		msg += _driDeviceFilePathName;

		throw Exception(Exception::Error::DRM_BAD_DEV_FILE, msg);
	}
}

Framebuffer* DrmDevice::generateFramebuffer(unsigned width, unsigned height)
{
	// TODO ... temp
	return 0;
}
