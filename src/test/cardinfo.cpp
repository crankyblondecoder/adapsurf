#include <iostream>
#include <stdlib.h>

#include "../DrmDevice.hpp"

using namespace std;

// The dri card number.
int cardNumber = 0;

// Outputs Linux DRM information for a specific card.
int main(int argc, char* argv[])
{
	if(argc > 1)
	{
		// The first param is the card number.
		cardNumber = atoi(argv[1]);
	}

	adapsurf::DrmDevice::enumerateDeviceResources(cardNumber, 1);
}
