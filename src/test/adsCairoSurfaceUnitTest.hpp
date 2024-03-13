#include <cmath>
#include <iostream>
#include <string>
#include <unistd.h>

#include "adsUnitTest.hpp"
#include "../CairoSurface.hpp"
#include "../DrmDevice.hpp"
#include "../Exception.hpp"

extern int cardNumber;
extern int verbosity;
extern int connectorNumber;
extern int modeNumber;

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

			// Catch all for all passed.
			if(allPassed) _notifyTestResult("Cairo Surface Tests", true, "All tests passed.");
		}

	private:

		DrmDevice* device = 0;

		bool allPassed = true;

		void constructDevice()
		{
			try
			{
				device = new DrmDevice(cardNumber, connectorNumber, modeNumber);
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
				CairoSurface* surface = 0;

				// Try and construct a cairo surface.
				try
				{
					surface = new CairoSurface(0, 0, device -> getDisplayResolutionWidth(),
						device -> getDisplayResolutionHeight());
				}
				catch(const Exception& ex)
				{
					allPassed = false;

					std::string msg("Exception during construction of cairo surface: ");
					msg += ex.getErrorDescr();

					_notifyTestResult("Cairo Surface Tests", false, msg);
				}

				if(surface)
				{
					// Make sure both buffers are cleared.
					device -> clear(0.0, 0.0, 0.0);
					device -> pageFlip();
					device -> clear(0.0, 0.0, 0.0);
					device -> pageFlip();

					// TODO ... Do some basic drawing.

					//sleep(2);
				}
			}
		}

		void destroyDevice()
		{
			if(device)
			{
				delete device;
			}
		}
};