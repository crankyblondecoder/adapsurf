#include "Device.hpp"

using namespace adapsurf;

Device::Device() : _clearColourSet{false}
{
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
