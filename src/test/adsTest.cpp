#include "adsDrmDeviceUnitTest.hpp"

#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	cout << "\n" << "*** Starting Adapsurf Unit Tests ***" << "\n\n";

	adsDrmDeviceUnitTest vectorUnitTest;
	vectorUnitTest.run();

	cout << "\n" << "*** Adapsurf Unit Tests complete ***" << "\n";
}