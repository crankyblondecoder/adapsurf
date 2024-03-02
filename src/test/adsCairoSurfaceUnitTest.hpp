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

class adsCairoSurfaceUnitTest : public adsUnitTest
{
	public:

		adsCairoSurfaceUnitTest() : adsUnitTest("Cairo Surface Tests") {}

	protected:

		virtual void _runTests()
		{
			constructDevice();

			if(allPassed)
			{
				testCairoSurface();
			}

			destroyDevice();
		}

	private:

		DrmDevice* device = 0;

		bool allPassed = true;

		void constructDevice()
		{
			try
			{
				device = new DrmDevice(cardNumber, connectorNumber);
			}
			catch(const Exception& ex)
			{
				allPassed = false;

				std::string msg("Exception during construction of drm device: ");
				msg += ex.getErrorDescr();

				_notifyTestResult("Cairo Surface Tests", false, msg);
			}
		}

		void testCairoSurface()
		{
			if(device)
			{
				device -> clear(1.0, 0.0, 0.0);
				device -> pageFlip();

				sleep(2);

				device -> clear(0.0, 1.0, 0.0);
				device -> pageFlip();

				sleep(2);

				device -> clear(0.0, 0.0, 1.0);
				device -> pageFlip();

				sleep(2);
			}

			// Catch all for all passed.
			if(allPassed) _notifyTestResult("DRM Device Construction", true, "All tests passed.");
		}

		void destroyDevice()
		{
			if(device)
			{
				delete device;
			}
		}
};