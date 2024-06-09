#include "Exception.hpp"
#include "Framebuffer.hpp"
#include "Surface.hpp"

using namespace adapsurf;

Surface::~Surface()
{
}

Surface::Surface(int localPositionX, int localPositionY)
	: _parent{0}, _children{0}, _childrenSize{0}, _numChildren{0}, _localPositionX{localPositionX},
	_localPositionY{localPositionY}, _globalPositionX{localPositionX}, _globalPositionY{localPositionY}
{
}

void Surface::compose(Framebuffer& framebuffer)
{
	// Composition starts with this surface and then traverses each child in turn. That way the children are composited last
	// with the last child being "on top" of everything else.

	framebuffer.compose(*this);

	Surface* child;

	for(unsigned index = 0; index < _childrenSize; index++)
	{
		child = _children[index];

		if(child) child -> compose(framebuffer);
	}
}

int Surface::getGlobalPositionX()
{
	return _globalPositionX;
}

int Surface::getGlobalPositionY()
{
	return _globalPositionY;
}

int Surface::_getGlobalPositionX()
{
	return _globalPositionX;
}

int Surface::_getGlobalPositionY()
{
	return _globalPositionY;
}

void Surface::addChild(Surface* child)
{
	if(_childrenSize == 0 || _numChildren == _childrenSize)
	{
		// Create a new page.
		unsigned newSize = _childrenSize + CHILD_ARRAY_PAGE_SIZE;

		Surface** newArray = new Surface*[newSize];

		for(unsigned index = 0; index < newSize; index++)
		{
			if(index < _childrenSize)
			{
				newArray[index] = _children[index];
			}
			else
			{
				newArray[index] = 0;
			}
		}

		// First slot in newly allocated page.
		newArray[_childrenSize] = child;

		_childrenSize = newSize;
		_numChildren++;

		delete[] _children;

		_children = newArray;
	}
	else
	{
		// Find first unsed slot for child.

		for(unsigned index = 0; index < _childrenSize; index++)
		{
			if(!_children[index])
			{
				_children[index] = child;
				_numChildren++;
				break;
			}
		}
	}
}

void Surface::removeChild(Surface* child)
{
	__removeChild(child);
}

void Surface::__removeChild(Surface* child)
{
	// NOTE: Does not shrink child array.

	bool childFound = false;

	if(_children && _numChildren > 0)
	{
		for(unsigned index = 0; index < _childrenSize; index++)
		{
			if(_children[index] == child)
			{
				_children[index] = 0;
				child -> __setParent(0);
				_numChildren--;
				childFound = true;
				break;
			}
		}
	}

	if(!childFound)
	{
		std::string msg("Could not find child that was requested for removal.");
		throw Exception(Exception::Error::SURF_CHILD_NOT_FOUND, msg);
	}
}

void Surface::__setParent(Surface* parent)
{
	_parent = parent;

	__updateGlobalPosn();
}

void Surface::__updateGlobalPosn()
{
	_globalPositionX = _localPositionX;
	_globalPositionY = _localPositionY;

	if(_parent)
	{
		_globalPositionX += _parent -> _getGlobalPositionX();
		_globalPositionY += _parent -> _getGlobalPositionY();
	}
}

uint8_t* Surface::getPixelData()
{
	return _getPixelData();
}

unsigned Surface::getPixelDataSize()
{
	return _getPixelDataStride() * getHeight();
}

unsigned Surface::getPixelDataStride()
{
	return _getPixelDataStride();
}
