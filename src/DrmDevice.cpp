#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "DrmDevice.hpp"
#include "Exception.hpp"

using namespace adapsurf;

DrmDevice::~DrmDevice()
{
	if(_drmResources) drmModeFreeResources(_drmResources);

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

	_drmResources = drmModeGetResources(_devFd);

	if(_drmResources == 0)
	{
		std::string msg("Could not get drm resources.");

		throw Exception(Exception::Error::DRM_GET_RESOURCES_FAIL, msg);
	}
}

void DrmDevice::enumerateResources()
{
	if(_drmResources)
	{
		cout << "DRM Resources\n";
		cout << "-------------\n";
		cout << "Min Width:" << _drmResources -> min_width << "  Max Width: " << _drmResources -> max_width << "\n";
		cout << "Min Height:" << _drmResources -> min_height << "  Max Height: " << _drmResources -> max_height << "\n";
	}
}

Framebuffer* DrmDevice::generateFramebuffer(unsigned width, unsigned height)
{
	// TODO ... temp
	return 0;
}
