#include <fcntl.h>

#include "DrmDevice.hpp"

using namespace adapsurf;

Framebuffer* DrmDevice::generateFramebuffer(unsigned width, unsigned height)
{
	_devFd = open(_driDeviceFilePathName, O_RDWR);

	if(_devFd < 0)
	{
		printf("Could not open dri device %s\n", _driDeviceFilePathName);
	}
}
