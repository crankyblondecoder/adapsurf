#include "adsDrmDeviceUnitTest.hpp"

#include <iostream>
#include <stdlib.h>

using namespace std;

int cardNumber = 0;

int main(int argc, char* argv[])
{
	if(argc > 1)
	{
		// Assume the first arg is the card number.
		cardNumber = atoi(argv[1]);
	}

	cout << "\n*** Starting Adapsurf Unit Tests ***\n\n";

	adsDrmDeviceUnitTest drmDeviceTest;
	drmDeviceTest.run();

	cout << "\n*** Adapsurf Unit Tests complete ***\n";
}
