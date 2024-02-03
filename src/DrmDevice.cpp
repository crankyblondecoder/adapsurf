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

DrmDevice::DrmDevice(unsigned cardNumber)
{
	_driDeviceFilePathName = "/dev/dri/card";
	_driDeviceFilePathName += to_string(cardNumber);

	_devFd = open(_driDeviceFilePathName.c_str(), O_RDWR);

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
		cout << "Framebuffer Count:" << _drmResources -> count_fbs << "\n";
		cout << "CRTC Count:" << _drmResources -> count_crtcs << "\n";
		cout << "Connector Count:" << _drmResources -> count_connectors << "\n";
		cout << "Encoder Count:" << _drmResources -> count_encoders << "\n\n";

		cout << "Connector Info:";
		for(int index = 0; index < _drmResources -> count_connectors; index++)
		{
			drmModeConnectorPtr connectPtr = drmModeGetConnector(_devFd, _drmResources -> connectors[index]);

			if(connectPtr)
			{
				cout << "\tType:" << connector_type_name(connectPtr -> connector_type)

				DRM_MODE_CONNECTOR
			}

			drmModeFreeConnector(connectPtr);
		}
	}
}

Framebuffer* DrmDevice::generateFramebuffer(unsigned width, unsigned height)
{
	// TODO ... temp
	return 0;
}

struct typeName
{
	unsigned int type;
	const char* name;
};

static const struct typeName connectorTypeNames[] =
{
    { DRM_MODE_CONNECTOR_Unknown, "unknown" },
    { DRM_MODE_CONNECTOR_VGA, "VGA" },
    { DRM_MODE_CONNECTOR_DVII, "DVI-I" },
    { DRM_MODE_CONNECTOR_DVID, "DVI-D" },
    { DRM_MODE_CONNECTOR_DVIA, "DVI-A" },
    { DRM_MODE_CONNECTOR_Composite, "composite" },
    { DRM_MODE_CONNECTOR_SVIDEO, "s-video" },
    { DRM_MODE_CONNECTOR_LVDS, "LVDS" },
    { DRM_MODE_CONNECTOR_Component, "component" },
    { DRM_MODE_CONNECTOR_9PinDIN, "9-pin DIN" },
    { DRM_MODE_CONNECTOR_DisplayPort, "DP" },
    { DRM_MODE_CONNECTOR_HDMIA, "HDMI-A" },
    { DRM_MODE_CONNECTOR_HDMIB, "HDMI-B" },
    { DRM_MODE_CONNECTOR_TV, "TV" },
    { DRM_MODE_CONNECTOR_eDP, "eDP" },
    { DRM_MODE_CONNECTOR_VIRTUAL, "Virtual" },
    { DRM_MODE_CONNECTOR_DSI, "DSI" },
    { DRM_MODE_CONNECTOR_DPI, "DPI" }
};

const char* connectorTypeName(unsigned int type)
{
    if(type < ARRAY_SIZE(connectorTypeNames) && type >= 0)
	{
        return connector_type_names[type].name;
    }

    return "INVALID";
}