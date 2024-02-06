#include "adsDrmDeviceUnitTest.hpp"

#include <iostream>
#include <stdlib.h>

using namespace std;

// The dri card number.
int cardNumber = 0;
// Verbosity level.
int verbosity = 0;

int main(int argc, char* argv[])
{
	if(argc > 1)
	{
		// Assume the first param is the card number.
		cardNumber = atoi(argv[1]);
	}

	if(argc > 2)
	{
		// Assume second param is the verbosity number.
		verbosity = atoi(argv[2]);
	}

	cout << "\n*** Starting Adapsurf Unit Tests ***\n\n";

	adsDrmDeviceUnitTest drmDeviceTest;
	drmDeviceTest.run();

	cout << "*** Adapsurf Unit Tests complete ***\n";
}
