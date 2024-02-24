#include <cmath>
#include <iostream>
#include <string>
#include <unistd.h>

#include "adsUnitTest.hpp"
#include "../DrmDevice.hpp"
#include "../Exception.hpp"

extern int cardNumber;
extern int verbosity;
extern int connectorNumber;

using namespace adapsurf;

class adsDrmDeviceUnitTest : public adsUnitTest
{
	public:

		adsDrmDeviceUnitTest() : adsUnitTest("DRM Device Test") {}

	protected:

		virtual void _runTests()
		{
			testDeviceConstruction();
		}

	private:

		void testDeviceConstruction()
		{
			bool allPassed = true;

			DrmDevice* device = 0;

			try
			{
				device = new DrmDevice(cardNumber, connectorNumber);
			}
			catch(const Exception& ex)
			{
				allPassed = false;

				std::string msg("Exception during construction: ");
				msg += ex.getErrorDescr();

				_notifyTestResult("DRM Device Tests", false, msg);
			}

			if(allPassed && verbosity > 0)
			{
				device -> enumerateResources(_getLevel() + 1);
				cout << "\n";
			}

			if(device)
			{
				device -> clear(1.0, 0.0, 0.0);
				device -> pageFlip();

				usleep(3000);

				delete device;
			}

			// Catch all for all passed.
			if(allPassed) _notifyTestResult("DRM Device Construction", true, "All tests passed.");
		}
};