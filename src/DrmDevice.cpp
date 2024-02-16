#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "DrmDevice.hpp"
#include "DrmFramebuffer.hpp"
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
	__dealloc();
}

void DrmDevice::__dealloc()
{
	if(_connectorAlloc)
	{
		drmModeFreeConnector(_connector);
		_connectorAlloc = false;
	}

	if(_drmResourcesAlloc)
	{
 		drmModeFreeResources(_drmResources);
		_drmResourcesAlloc = false;
	}

	if(_deviceFdAlloc)
	{
		close(_deviceFd);
		_deviceFdAlloc = false;
	}
}

DrmDevice::DrmDevice(unsigned cardNumber, int connectorIndex)
	: _deviceFdAlloc{false}, _drmResourcesAlloc{false}, _connectorAlloc{false}
{
	_driDeviceFilePathName = "/dev/dri/card";
	_driDeviceFilePathName += to_string(cardNumber);

	_deviceFd = open(_driDeviceFilePathName.c_str(), O_RDWR);

	if(_deviceFd < 0)
	{
		std::string msg("Could not open dri device: ");
		msg += _driDeviceFilePathName;

		throw Exception(Exception::Error::DRM_BAD_DEV_FILE, msg);
	}

	_deviceFdAlloc = true;

	// Get available drm resources.

	_drmResources = drmModeGetResources(_deviceFd);

	if(_drmResources == 0)
	{
		__dealloc();

		std::string msg("Could not get drm resources.");
		throw Exception(Exception::Error::DRM_GET_RESOURCES_FAIL, msg);
	}

	_drmResourcesAlloc = true;

	// Get dumb buffer capabilities.

	uint64_t capValue;

	if(!drmGetCap(_deviceFd, DRM_CAP_DUMB_BUFFER, &capValue)) {

		_dumbBufferSupport = capValue > 0;

	} else {

		__dealloc();

		std::string msg("Could not get drm capability: DRM_CAP_DUMB_BUFFER");
		throw Exception(Exception::Error::DRM_GET_CAP_FAIL, msg);
	}

	if(!drmGetCap(_deviceFd, DRM_CAP_DUMB_PREFERRED_DEPTH, &capValue)) {

		_dumbBufferPrefDepth = capValue;

	} else {

		__dealloc();

		std::string msg("Could not get drm capability: DRM_CAP_DUMB_PREFERRED_DEPTH");
		throw Exception(Exception::Error::DRM_GET_CAP_FAIL, msg);
	}

	// The requested connector is processed during framebuffer generation.
	if(connectorIndex < 0)
	{
		// Look for the first connected connector.
		for(int index = 0; index < _drmResources -> count_connectors; index++)
		{
			drmModeConnectorPtr connectPtr = drmModeGetConnector(_deviceFd, _drmResources -> connectors[index]);

			if(connectPtr && connectPtr -> connection == drmModeConnection::DRM_MODE_CONNECTED)
			{
				_connector = connectPtr;
				_connectorAlloc = true;
				break;
			}

			drmModeFreeConnector(connectPtr);
		}

		if(_connectorIndex == -1)
		{
			std::string msg("Could not find a suitable, connected, default connector.");
			throw Exception(Exception::Error::DRM_GET_DEFAULT_CONNECT_FAIL, msg);
		}
	}
	else
	{
		_connectorIndex = connectorIndex;
	}
}

Framebuffer* DrmDevice::generateFramebuffer()
{
	if(_connectorIndex < 0)
	{
		std::string msg("Connector index has not been defined.");
		throw Exception(Exception::Error::DRM_CREATE_FRAME_BUFFER_FAIL, msg);
	}

	if(!_dumbBufferSupport)
	{
		std::string msg("Dumb buffer is not supported. Could not create framebuffer.");
		throw Exception(Exception::Error::DRM_GET_CAP_FAIL, msg);
	}

	if(_dumbBufferPrefDepth < 24)
	{
		std::string msg("A depth of less then 24 is not currently supported.");
		throw Exception(Exception::Error::DRM_CREATE_FRAME_BUFFER_FAIL, msg);
	}

	drmModeConnectorPtr connectPtr = drmModeGetConnector(_deviceFd, _drmResources -> connectors[_connectorIndex]);

	if(!connectPtr)
	{
		std::string msg("Unable to get connector information.");
		throw Exception(Exception::Error::DRM_CREATE_FRAME_BUFFER_FAIL, msg);
	}

	if(!(connectPtr -> connection == drmModeConnection::DRM_MODE_CONNECTED))
	{
		drmModeFreeConnector(connectPtr);

		std::string msg("Requested connection: ");
		msg += _connectorIndex + " is not connected and cannot be safely queried.";
		throw Exception(Exception::Error::DRM_CREATE_FRAME_BUFFER_FAIL, msg);
	}

	if(connectPtr -> count_modes < 1)
	{
		drmModeFreeConnector(connectPtr);

		std::string msg("At least one connector mode could not be found.");
		throw Exception(Exception::Error::DRM_CREATE_FRAME_BUFFER_FAIL, msg);
	}

	// For now just set width and height to the first mode found.
	uint32_t width = connectPtr -> modes[0].hdisplay;
	uint32_t height = connectPtr -> modes[0].vdisplay;

	// TODO ... To set a framebuffer as being displayed, the crtcId, connector pointer and mode pointer from the connector are required.
	//          these must be the same for all framebuffers produced from this device.
	//          Maybe this data should be created as part of the constructor?
	blah;

	// Free resources that are no longer needed.
	drmModeFreeConnector(connectPtr);

	return new DrmFramebuffer(_deviceFd, width, height, _dumbBufferPrefDepth, 32);
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
			drmModeConnectorPtr connectPtr = drmModeGetConnector(_deviceFd, _drmResources -> connectors[index]);

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
