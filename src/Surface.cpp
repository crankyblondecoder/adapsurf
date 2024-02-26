#include "Exception.hpp"
#include "Surface.hpp"

using namespace adapsurf;

Surface::~Surface()
{
}

Surface::Surface()
	: __parent{0}, __children{0}, __childrenSize{0}, __numChildren{0}
{
}

unsigned Surface::getPositionX()
{
	return __positionX;
}

unsigned Surface::getPositionY()
{
	return __positionY;
}

unsigned Surface::getWidth()
{
	return __curWidth;
}

unsigned Surface::getHeight()
{
	return __curHeight;
}

void Surface::addChild(Surface* child)
{
	if(__childrenSize == 0 || __numChildren == __childrenSize)
	{
		// Create a new page.
		unsigned newSize = __childrenSize + CHILD_ARRAY_PAGE_SIZE;

		Surface** newArray = new Surface*[newSize];

		for(unsigned index = 0; index < newSize; index++)
		{
			if(index < __childrenSize)
			{
				newArray[index] = __children[index];
			}
			else
			{
				newArray[index] = 0;
			}
		}

		// First slot in newly allocated page.
		newArray[__childrenSize] = child;

		__childrenSize = newSize;
		__numChildren++;

		delete[] __children;

		__children = newArray;
	}
	else
	{
		// Find first unsed slot for child.

		for(unsigned index = 0; index < __childrenSize; index++)
		{
			if(!__children[index])
			{
				__children[index] = child;
				__numChildren++;
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

	if(__children && __numChildren > 0)
	{
		for(unsigned index = 0; index < __childrenSize; index++)
		{
			if(__children[index] == child)
			{
				__children[index] = 0;
				child -> __setParent(0);
				__numChildren--;
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
	__parent = parent;
}
