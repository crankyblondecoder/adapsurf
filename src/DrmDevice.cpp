#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "DrmDevice.hpp"
#include "Exception.hpp"

using namespace adapsurf;

struct typeName
{
	unsigned int type;
	const char* name;
};

static const struct typeName connectorTypeNames[] =
{
    {DRM_MODE_CONNECTOR_Unknown, "unknown"},
    {DRM_MODE_CONNECTOR_VGA, "VGA"},
    {DRM_MODE_CONNECTOR_DVII, "DVI-I"},
    {DRM_MODE_CONNECTOR_DVID, "DVI-D"},
    {DRM_MODE_CONNECTOR_DVIA, "DVI-A"},
    {DRM_MODE_CONNECTOR_Composite, "composite"},
    {DRM_MODE_CONNECTOR_SVIDEO, "s-video"},
    {DRM_MODE_CONNECTOR_LVDS, "LVDS"},
    {DRM_MODE_CONNECTOR_Component, "component"},
    {DRM_MODE_CONNECTOR_9PinDIN, "9-pin DIN"},
    {DRM_MODE_CONNECTOR_DisplayPort, "DP"},
    {DRM_MODE_CONNECTOR_HDMIA, "HDMI-A"},
    {DRM_MODE_CONNECTOR_HDMIB, "HDMI-B"},
    {DRM_MODE_CONNECTOR_TV, "TV"},
    {DRM_MODE_CONNECTOR_eDP, "eDP"},
    {DRM_MODE_CONNECTOR_VIRTUAL, "Virtual"},
    {DRM_MODE_CONNECTOR_DSI, "DSI"},
    {DRM_MODE_CONNECTOR_DPI, "DPI"},
	{DRM_MODE_CONNECTOR_WRITEBACK, "WRITEBACK"},
	{DRM_MODE_CONNECTOR_SPI, "SPI"},
	{DRM_MODE_CONNECTOR_USB, "USB"}
};

const char* connectorTypeName(unsigned int type)
{
    if(type < DRM_MODE_CONNECTOR_USB)
	{
        return connectorTypeNames[type].name;
    }

    return "INVALID";
}

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

void DrmDevice::enumerateResources(unsigned prefTabNum)
{
	string prefixTabs;

	for(unsigned index = 0; index < prefTabNum; index++)
	{
		prefixTabs += "\t";
	}

	if(_drmResources)
	{
		cout << prefixTabs << "DRM Resources\n";
		cout << prefixTabs << "-------------\n";
		cout << prefixTabs << "Device: " << _driDeviceFilePathName << "\n";
		cout << prefixTabs << "Min Width: " << _drmResources -> min_width << "  Max Width: " << _drmResources -> max_width << "\n";
		cout << prefixTabs << "Min Height: " << _drmResources -> min_height << "  Max Height: " << _drmResources -> max_height << "\n";
		cout << prefixTabs << "Framebuffer Count: " << _drmResources -> count_fbs << "\n";
		cout << prefixTabs << "CRTC Count: " << _drmResources -> count_crtcs << "\n";
		cout << prefixTabs << "Connector Count: " << _drmResources -> count_connectors << "\n";
		cout << prefixTabs << "Encoder Count: " << _drmResources -> count_encoders << "\n\n";

		cout << prefixTabs << "Connector Info:\n";
		for(int index = 0; index < _drmResources -> count_connectors; index++)
		{
			drmModeConnectorPtr connectPtr = drmModeGetConnector(_devFd, _drmResources -> connectors[index]);

			if(connectPtr)
			{
				cout << prefixTabs << "\tType:" << connectorTypeName(connectPtr -> connector_type) << "\n";
				cout << prefixTabs << "\tmmWidth:" << connectPtr -> mmWidth << "\n";
				cout << prefixTabs << "\tmmHeight:" << connectPtr -> mmHeight << "\n";

				if(connectPtr -> connection == drmModeConnection::DRM_MODE_CONNECTED)
				{
					cout << prefixTabs << "\tConnectionStatus: CONNECTED" << "\n";
				}
				else if(connectPtr -> connection == drmModeConnection::DRM_MODE_DISCONNECTED)
				{
					cout << prefixTabs << "\tConnectionStatus: DISCONNECTED" << "\n";
				}

				drmModeModeInfo mode;

				for(int modeIndex = 0; modeIndex < connectPtr -> count_modes; modeIndex++)
				{
					mode = connectPtr -> modes[modeIndex];

					if(mode.type & DRM_MODE_TYPE_PREFERRED)
					{
						cout << prefixTabs << "\tPref horiz display size: " << mode.hdisplay << "\n";
						cout << prefixTabs << "\tPref vert display size: " << mode.vdisplay << "\n";
					}
				}
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