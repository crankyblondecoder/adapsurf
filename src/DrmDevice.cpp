#include <cstring>
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
	// TODO ... Destroy any allocated frame buffers.

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

	// Get available drm resources.

	_drmResources = drmModeGetResources(_devFd);

	if(_drmResources == 0)
	{
		std::string msg("Could not get drm resources.");

		throw Exception(Exception::Error::DRM_GET_RESOURCES_FAIL, msg);
	}

	// Get dumb buffer capabilities.

	uint64_t capValue;

	if(!drmGetCap(_devFd, DRM_CAP_DUMB_BUFFER, &capValue)) {

		_dumbBufferSupport = capValue > 0;

	} else {

		std::string msg("Could not get drm capability: DRM_CAP_DUMB_BUFFER");

		throw Exception(Exception::Error::DRM_GET_CAP_FAIL, msg);
	}

	if(!drmGetCap(_devFd, DRM_CAP_DUMB_PREFERRED_DEPTH, &capValue)) {

		_dumbBufferPrefDepth = capValue;

	} else {

		std::string msg("Could not get drm capability: DRM_CAP_DUMB_PREFERRED_DEPTH");

		throw Exception(Exception::Error::DRM_GET_CAP_FAIL, msg);
	}
}

/*
Framebuffer* DrmDevice::generateFramebuffer(unsigned width, unsigned height)
{
	struct drm_mode_create_dumb createReq;

	std::memset(&createReq, 0, sizeof(createReq));
}

void DrmDevice::destroyFramebuffer(Framebuffer* fbuf)
{

}
*/
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
		cout << prefixTabs << "Dumb buffer support: " << (_dumbBufferSupport ? "true" : "false") << "\n";
		cout << prefixTabs << "Dumb buffer pref depth: " << _dumbBufferPrefDepth << "\n";
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

					cout << prefixTabs << "\tMode Index: " << modeIndex << "\n";

					if(mode.type & DRM_MODE_TYPE_PREFERRED)
					{
						cout << prefixTabs << "\t\tPref horiz display size: " << mode.hdisplay << "\n";
						cout << prefixTabs << "\t\tPref vert display size: " << mode.vdisplay << "\n";
					}

					if(mode.type & DRM_MODE_TYPE_DRIVER)
					{
						cout << prefixTabs << "\t\tDriver horiz display size: " << mode.hdisplay << "\n";
						cout << prefixTabs << "\t\tDriver vert display size: " << mode.vdisplay << "\n";
					}

					if(mode.type & DRM_MODE_TYPE_USERDEF)
					{
						cout << prefixTabs << "\t\tUserdef horiz display size: " << mode.hdisplay << "\n";
						cout << prefixTabs << "\t\tUserdef vert display size: " << mode.vdisplay << "\n";
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