#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <poll.h>
#include <string>
#include <unistd.h>

using namespace std;

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
	if(_prevCrtc && _connectorAlloc)
	{
		// Restore state of crtc prior to this device taking over.

		drmModeSetCrtc(_deviceFd, _prevCrtc -> crtc_id, _prevCrtc -> buffer_id, _prevCrtc -> x, _prevCrtc -> y,
			&(_connector -> connector_id), 1, &(_prevCrtc -> mode));
	}

	__dealloc();
}

void DrmDevice::__dealloc()
{
	if(_fb1) { delete _fb1; _fb1 = 0; }
	if(_fb2) { delete _fb2; _fb2 = 0; }
	if(_fb3) { delete _fb3; _fb3 = 0; }

	if(_prevCrtc) drmModeFreeCrtc(_prevCrtc);

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

DrmDevice::DrmDevice(unsigned cardNumber, int connectorIndex, int modeIndex)
	: _deviceFdAlloc{false}, _drmResourcesAlloc{false}, _connectorAlloc{false}, _connectorMode{0}, _fb1{0},
	_fb2{0}, _fb3{0}, _curFbNum{0}
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

		if(capValue == 0)
		{
			cout << "Warning: Dumb buffer preferred depth is 0. A depth of 24 will be assumed.\n";

			// Default to 24 bits.
			_dumbBufferPrefDepth = 24;
		}
		else
		{
			_dumbBufferPrefDepth = capValue;
		}

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

		if(!_connectorAlloc)
		{
			__dealloc();

			std::string msg("Could not find a suitable, connected, default connector.");
			throw Exception(Exception::Error::DRM_GET_DEFAULT_CONNECT_FAIL, msg);
		}
	}
	else
	{
		_connector = drmModeGetConnector(_deviceFd, _drmResources -> connectors[connectorIndex]);

		if(!_connector)
		{
			__dealloc();

			std::string msg("Could not find requested connector with index: ");
			msg += std::to_string(connectorIndex);
			throw Exception(Exception::Error::DRM_GET_DEFAULT_CONNECT_FAIL, msg);
		}

		_connectorAlloc = true;
	}

	// Look for suitable mode that connector publishes.
	// Use specified mode index and only fall back to preferred mode if not specified.

	if(modeIndex >= 0 && modeIndex < _connector -> count_modes)
	{
		_connectorMode = _connector -> modes + modeIndex;
	}
	else
	{
		for(int modeIdx = 0; modeIdx < _connector -> count_modes; modeIdx++)
		{
			drmModeModeInfoPtr mode = _connector -> modes + modeIdx;

			if(mode -> type & DRM_MODE_TYPE_PREFERRED)
			{
				_connectorMode = mode;
				break;
			}

			// Use the first found node unless a preferred node is found.
			if(!_connectorMode) _connectorMode = mode;
		}
	}

	if(!_connectorMode)
	{
		__dealloc();

		std::string msg("At least one connector mode could not be found.");
		throw Exception(Exception::Error::DRM_FIND_CONNECT_MODE_FAIL, msg);
	}

	// NOTE: To set a framebuffer as being displayed, the crtcId, connector pointer and mode pointer from the connector
	// are required. These must be the same for all framebuffers produced from this device.

	// Look for a CRTC that can scan out the framebuffers.
	bool crtcFound = false;

	drmModeEncoderPtr encoder;

	for(int encoderIndex = 0; encoderIndex < _drmResources -> count_encoders; encoderIndex++)
	{
		encoder = drmModeGetEncoder(_deviceFd, _connector -> encoders[encoderIndex]);

		if(encoder)
		{
			for(int crtcIndex = 0; crtcIndex < _drmResources -> count_crtcs; crtcIndex++)
			{
				if(encoder -> possible_crtcs & (1 << crtcIndex))
				{
					_crtcId = _drmResources -> crtcs[crtcIndex];
					crtcFound = true;
					break;
				}
			}

			drmModeFreeEncoder(encoder);
		}

		if(crtcFound) break;
	}

	if(!crtcFound)
	{
		__dealloc();

		std::string msg("At least one usable CRTC could not be found.");
		throw Exception(Exception::Error::DRM_CREATE_FRAME_BUFFER_FAIL, msg);
	}
	else
	{
		// Save the crtc state so it can restored on this being destroyed.
		_prevCrtc = drmModeGetCrtc(_deviceFd, _crtcId);
	}

	// Generate framebuffers.

	try
	{
		_fb1 = __generateFramebuffer();
		_fb2 = __generateFramebuffer();
		_fb3 = __generateFramebuffer();
	}
	catch(const Exception& ex)
	{
		__dealloc();

		throw ex;
	}

	// Device should now be fully setup. The only thing left is to assign the fb's to a crtc as requried.
}

DrmFramebuffer* DrmDevice::__generateFramebuffer()
{
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

	// Use the connector modes resolution.
	uint32_t width = _connectorMode -> hdisplay;
	uint32_t height = _connectorMode -> vdisplay;

	return new DrmFramebuffer(_deviceFd, width, height, _dumbBufferPrefDepth, 32);
}

void DrmDevice::enumerateResources(unsigned prefTabNum)
{
	string prefixTabs;

	for(unsigned index = 0; index < prefTabNum; index++)
	{
		prefixTabs += "\t";
	}

	if(_deviceFd && _drmResources)
	{
		cout << prefixTabs << "DRM Resources\n";
		cout << prefixTabs << "-------------\n";
		cout << prefixTabs << "Device: " << _driDeviceFilePathName << "\n";
		cout << prefixTabs << "Dumb buffer support: " << (_dumbBufferSupport ? "true" : "false") << "\n";
		cout << prefixTabs << "Dumb buffer pref depth: " << _dumbBufferPrefDepth << "\n";
		if(_fb1) cout << prefixTabs << "Framebuffer fourcc: " << _fb1 -> getFourcc() << "\n";
		cout << prefixTabs << "Min Width: " << _drmResources -> min_width << "  Max Width: " << _drmResources -> max_width << "\n";
		cout << prefixTabs << "Min Height: " << _drmResources -> min_height << "  Max Height: " << _drmResources -> max_height << "\n";
		cout << prefixTabs << "Framebuffer Count: " << _drmResources -> count_fbs << "\n";
		cout << prefixTabs << "CRTC Count: " << _drmResources -> count_crtcs << "\n";
		cout << prefixTabs << "Connector Count: " << _drmResources -> count_connectors << "\n";
		cout << prefixTabs << "Encoder Count: " << _drmResources -> count_encoders << "\n\n";

		cout << prefixTabs << "Connector Info:\n\n";
		for(int index = 0; index < _drmResources -> count_connectors; index++)
		{
			drmModeConnectorPtr connectPtr = drmModeGetConnector(_deviceFd, _drmResources -> connectors[index]);

			if(connectPtr)
			{
				cout << prefixTabs << "\tConnector Index: " << index << "\n";
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

void DrmDevice::enumerateDeviceResources(unsigned cardNumber, unsigned prefTabNum)
{
	cout << "\n";

	string prefixTabs;

	for(unsigned index = 0; index < prefTabNum; index++)
	{
		prefixTabs += "\t";
	}

	string driDeviceFilePathName = "/dev/dri/card";
	driDeviceFilePathName += to_string(cardNumber);

	int deviceFd = open(driDeviceFilePathName.c_str(), O_RDWR);

	if(deviceFd < 0)
	{
		string msg = "Could not open device:";
		msg += driDeviceFilePathName;

		cout << msg << "\n";
	}

	drmModeResPtr drmResources = drmModeGetResources(deviceFd);

	if(!drmResources)
	{
		string msg = "Could not get drm resources for card: ";
		msg += driDeviceFilePathName;

		cout << msg << "\n";
	}

	if(deviceFd && drmResources)
	{
		uint64_t capValue;

		cout << prefixTabs << "DRM Resources\n";
		cout << prefixTabs << "-------------\n";
		cout << prefixTabs << "Device: " << driDeviceFilePathName << "\n";

		if(!drmGetCap(deviceFd, DRM_CAP_DUMB_BUFFER, &capValue)) {

			bool dumbBufferSupport = capValue > 0;
			cout << prefixTabs << "Dumb buffer support: " << (dumbBufferSupport ? "true" : "false") << "\n";
		}

		if(!drmGetCap(deviceFd, DRM_CAP_DUMB_PREFERRED_DEPTH, &capValue)) {

			bool dumbBufferPrefDepth = capValue;
			cout << prefixTabs << "Dumb buffer pref depth: " << dumbBufferPrefDepth << "\n";
		}

		cout << prefixTabs << "Min Width: " << drmResources -> min_width << "  Max Width: " << drmResources -> max_width << "\n";
		cout << prefixTabs << "Min Height: " << drmResources -> min_height << "  Max Height: " << drmResources -> max_height << "\n";
		cout << prefixTabs << "Framebuffer Count: " << drmResources -> count_fbs << "\n";
		cout << prefixTabs << "CRTC Count: " << drmResources -> count_crtcs << "\n";
		cout << prefixTabs << "Connector Count: " << drmResources -> count_connectors << "\n";
		cout << prefixTabs << "Encoder Count: " << drmResources -> count_encoders << "\n\n";

		cout << prefixTabs << "Connector Info:\n\n";
		for(int index = 0; index < drmResources -> count_connectors; index++)
		{
			drmModeConnectorPtr connectPtr = drmModeGetConnector(deviceFd, drmResources -> connectors[index]);

			if(connectPtr)
			{
				cout << prefixTabs << "\tConnector Index: " << index << "\n";
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

					cout << prefixTabs << "\t\tvrefresh: " << mode.vrefresh << "\n";

					if(mode.type & DRM_MODE_TYPE_PREFERRED)
					{
						cout << prefixTabs << "\t\tPref horiz display size: " << mode.hdisplay << "\n";
						cout << prefixTabs << "\t\tPref vert display size: " << mode.vdisplay << "\n";
					}

					if(mode.type & DRM_MODE_TYPE_DRIVER)
					{
						cout << prefixTabs << "\t\tDriver horiz display size: " << mode.hdisplay << "\n";
						cout << prefixTabs << "\t\tDriver vert display size: " << mode.vdisplay << "\n";
						cout << prefixTabs << "\t\tDriver display size (h/v) ratio: " << (double)mode.hdisplay / (double)mode.vdisplay << "\n";
					}

					if(mode.type & DRM_MODE_TYPE_USERDEF)
					{
						cout << prefixTabs << "\t\tUserdef horiz display size: " << mode.hdisplay << "\n";
						cout << prefixTabs << "\t\tUserdef vert display size: " << mode.vdisplay << "\n";
					}
				}

				cout << "\n";
			}

			drmModeFreeConnector(connectPtr);
		}
	}

	if(drmResources)
	{
		drmModeFreeResources(drmResources);
	}

	if(deviceFd >= 0)
	{
		close(deviceFd);
	}

	cout << "\n";
}

void DrmDevice::pageFlipEventHandler(int fd, unsigned sequence, unsigned tv_sec, unsigned tv_usec, void* userData)
{
	if(userData)
	{
		((DrmDevice*) userData) -> _pageFlipping = false;
	}
}

void DrmDevice::pageFlip()
{
	if(!_deviceFd) return;

	if(drmSetMaster(_deviceFd) == -1)
	{
		std::string msg("Could not set device as master. IOCTL error number: ");
		msg += std::to_string(errno);
		throw Exception(Exception::Error::DRM_CANT_SET_MASTER, msg);
	}

	bool forceFlip = false;

	if(_pageFlipping)
	{
		// Have to wait for DRM_MODE_PAGE_FLIP_EVENT or until a reasonable timeout occurs.
		// If the timeout or error occurs just force an immediate no vysnc page flip.

		struct pollfd toPoll =
		{
			.fd = _deviceFd,
			.events = POLLIN
		};

		// Timeout is set to 33 milliseconds which corresponds to 30 FPS.

		int ret = poll(&toPoll, 1, 33);

		// Anything other than success just forces a flip.
		if(ret > 0 && toPoll.revents & POLLIN)
		{
			// Ready to attempt drm event read.
			drmEventContext event =
			{
				.version = 2,
				.page_flip_handler = pageFlipEventHandler
			};

			int eventRet = drmHandleEvent(_deviceFd, &event);

			if(eventRet < 0) forceFlip = true;
		}
		else
		{
			forceFlip = true;
		}
	}

	bool initialFlip = _curFbNum == 0;

	DrmFramebuffer* nextFb;

	if(_curFbNum == 0 || _curFbNum == 3)
	{
		_curFbNum = 1;
		nextFb = _fb1;
	}
	else if(_curFbNum == 1)
	{
		_curFbNum = 2;
		nextFb = _fb2;
	}
	else
	{
		_curFbNum = 3;
		nextFb = _fb3;
	}

	if(!(initialFlip || forceFlip))
	{
		// The "this" pointer passed is given back to the drm even handler.
		// Return value is the negative of the error number.
		int retVal = drmModePageFlip(_deviceFd, _crtcId, nextFb -> getFramebufferId(), DRM_MODE_PAGE_FLIP_EVENT, this);

		if(retVal >= 0)
		{
			_pageFlipping = true;
		}
		else
		{
			forceFlip = true;
		}
	}

	if(initialFlip || forceFlip)
	{
		// Return value is the negative of the error number.
		drmModeSetCrtc(_deviceFd, _crtcId, nextFb -> getFramebufferId(), 0, 0, &(_connector -> connector_id), 1, _connectorMode);
		_pageFlipping = false;
	}

	// Just ignore any error for now.
	drmDropMaster(_deviceFd);

	// Clear the new current back buffer if a clear colour has been set.
	if(_getClearColourSet())
	{
		DrmFramebuffer* backBuf = __getBackBuffer();

		if(backBuf)
		{
			backBuf -> clear(_getClearColourRed(), _getClearColourGreen(), _getClearColourBlue());
		}
	}
}

DrmFramebuffer* DrmDevice::__getBackBuffer()
{
	if(_curFbNum == 1)
	{
		return _fb2;
	}
	else if(_curFbNum == 2)
	{
		return _fb3;
	}
	else if(_curFbNum == 3)
	{
		return _fb1;
	}

	return _fb1;
}

void DrmDevice::clear(double red, double green, double blue)
{
	DrmFramebuffer* backBuf = __getBackBuffer();

	if(backBuf)
	{
		backBuf -> clear(red, green, blue);
	}
}

uint32_t DrmDevice::getDisplayResolutionWidth()
{
	if(_connectorMode) return _connectorMode -> hdisplay;

	return 0;
}

uint32_t DrmDevice::getDisplayResolutionHeight()
{
	if(_connectorMode) return _connectorMode -> vdisplay;

	return 0;
}

Framebuffer* DrmDevice::getDrawToFramebuffer()
{
	return __getBackBuffer();
}