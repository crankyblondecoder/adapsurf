#include <cmath>
#include <iostream>
#include <string>

#include "adsUnitTest.hpp"
#include "../DrmDevice.hpp"
#include "../Exception.hpp"

extern int cardNumber;

using namespace adapsurf;

class adsDrmDeviceUnitTest : public adsUnitTest
{
	public:

		adsDrmDeviceUnitTest() : adsUnitTest("DRM Device Test") {}

	protected:

		virtual void runTests()
		{
			testDeviceConstruction();
		}

	private:

		void testDeviceConstruction()
		{
			bool allPassed = true;

			DrmDevice* device;

			try
			{
				device = new DrmDevice(cardNumber);
			}
			catch(const Exception& ex)
			{
				allPassed = false;

				std::string msg("Exception during construction: ");
				msg += ex.getErrorDescr();

				notifyTestResult("DRM Device Tests", false, msg);
			}

			cout << "\n";
			if(allPassed) device -> enumerateResources();
			cout << "\n";

			delete device;

			// Catch all for all passed.
			if(allPassed) notifyTestResult("DRM Device Construction", true, "All tests passed.");
		}
};