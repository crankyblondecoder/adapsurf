#include "Device.hpp"

using namespace adapsurf;

Device::~Device()
{
}

Device::Device() : _clearColourSet{false}
{
}

void Device::setClearColour(double red, double green, double blue)
{
	_clearColourRed = red;
	_clearColourGreen = green;
	_clearColourBlue = blue;
	_clearColourSet = true;
}

bool Device::_getClearColourSet()
{
	return _clearColourSet;
}

double Device::_getClearColourRed()
{
	return _clearColourRed;
}

double Device::_getClearColourGreen()
{
	return _clearColourGreen;
}

double Device::_getClearColourBlue()
{
	return _clearColourBlue;
}
