#include "adsCairoSurfaceUnitTest.hpp"
#include "adsDrmDeviceUnitTest.hpp"

#include <iostream>
#include <stdlib.h>

using namespace std;

// The dri card number.
int cardNumber = 0;

// The dri connector number on the card.
int connectorNumber = 0;

// The mode number on the connector to use.
int modeNumber = 0;

// Verbosity level.
int verbosity = 1;

int main(int argc, char* argv[])
{
	if(argc > 1)
	{
		// The first param is the verbosity number.
		verbosity = atoi(argv[1]);
	}

	if(argc > 2)
	{
		// The second param is the card number.
		cardNumber = atoi(argv[2]);
	}

	if(argc > 3)
	{
		// The third param is the connector number.
		connectorNumber = atoi(argv[3]);
	}

	if(argc > 4)
	{
		// The fourth param is the mode number.
		connectorNumber = atoi(argv[4]);
	}

	cout << "\n*** Starting Adapsurf Unit Tests ***\n\n";

	adsDrmDeviceUnitTest drmDeviceTest;
	drmDeviceTest.run();

	//adsCairoSurfaceUnitTest cairoSurfaceTest;
	//cairoSurfaceTest.run();

	cout << "*** Adapsurf Unit Tests complete ***\n";
}
