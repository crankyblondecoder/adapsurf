#include "adsDrmDeviceUnitTest.hpp"

#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	cout << "\n" << "*** Starting libSNL Unit Tests ***" << "\n\n";

	adsDrmDeviceUnitTest vectorUnitTest;
	vectorUnitTest.run();

	cout << "\n" << "*** libSNL Unit Tests complete ***" << "\n";
}