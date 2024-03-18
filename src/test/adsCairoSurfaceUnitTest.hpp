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
				uint32_t dispWidth = device -> getDisplayResolutionWidth();
				uint32_t dispHeight = device -> getDisplayResolutionHeight();

				// Make sure both buffers are cleared.
				device -> clear(0.0, 0.0, 0.0);
				device -> pageFlip();
				device -> clear(0.0, 0.0, 0.0);
				device -> pageFlip();

				TestDraw1* surf1 = 0;
				TestDraw1* surf2 = 0;

				// Try and construct a cairo surface.
				try
				{
					surf1 = new TestDraw1(dispWidth, dispHeight);
					surf2 = new TestDraw1(dispWidth, dispHeight);
				}
				catch(const Exception& ex)
				{
					allPassed = false;

					std::string msg("Exception during construction of cairo test surface: ");
					msg += ex.getErrorDescr();

					_notifyTestResult("Cairo Surface Tests", false, msg);
				}

				if(surf1)
				{
					surf1 -> clear(1.0, 1.0, 1.0, 1.0);
					surf1 -> draw1();
					surf1 -> draw2();
					Framebuffer* drawToBuf = device -> getDrawToFramebuffer();
					drawToBuf -> compose(*surf1);
					device -> pageFlip();
					sleep(2);
				}

				if(surf1 && surf2)
				{
					surf1 -> clear(1.0, 1.0, 1.0, 1.0);
					surf1 -> draw1();
					surf2 -> clear(0.0, 0.0, 0.0, 0.0);
					surf2 -> draw2();
					Framebuffer* drawToBuf = device -> getDrawToFramebuffer();
					drawToBuf -> compose(*surf1);
					drawToBuf -> compose(*surf2);
					device -> pageFlip();
					sleep(2);
				}

				if(surf1) delete surf1;
				if(surf2) delete surf2;
			}
		}

		void destroyDevice()
		{
			if(device)
			{
				delete device;
			}
		}

		class TestDraw1 : public CairoSurface
		{
			public:

				TestDraw1(uint32_t dispWidth, uint32_t dispHeight) : CairoSurface(0, 0, dispWidth, dispHeight)
				{
				}

				virtual ~TestDraw1()
				{
				}

				void draw1()
				{
					cairo_t* ctx = _getContext();

					if(ctx)
					{
						// RGB 252, 165, 3
						cairo_set_source_rgba(ctx, 252.0/255.0, 165.0/255.0, 3.0/255.0, 0.5);
						cairo_rectangle(ctx, 0, 0, _getWidth() * 3 / 4, _getHeight() * 3 / 4);
						cairo_fill(ctx);
					}
				}

				void draw2()
				{
					cairo_t* ctx = _getContext();

					if(ctx)
					{
						// RGB 252, 165, 3
						cairo_set_source_rgba(ctx, 252.0/255.0, 165.0/255.0, 3.0/255.0, 0.5);
						cairo_rectangle(ctx, _getWidth() / 4, _getHeight() / 4, _getWidth() * 3 / 4, _getHeight() * 3 / 4);
						cairo_fill(ctx);
					}
				}
		};
};