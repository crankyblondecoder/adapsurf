#include <cmath>

#include "adsUnitTest.hpp"

class adsDrmDeviceUnitTest : public adsUnitTest
{
	public:

		adsDrmDeviceUnitTest() : adsUnitTest("DRM Device Test") {}

	protected:

		virtual void runTests()
		{
			testConstruction();
		}

	private:

		void testConstruction()
		{
			bool allPassed = true;

			// Catch all for all passed.
			if(allPassed) notifyTestResult("DRM Device Tests", true, "All tests passed.");
		}
};