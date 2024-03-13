#ifndef ADS_DEVICE_H
#define ADS_DEVICE_H

#include <cstdint>

#include "Framebuffer.hpp"

namespace adapsurf {

	/**
	 * A device abstracts a specific display device and provides a framebuffer into which drawing can take place.
	 */
	class Device
	{
		public:

			virtual ~Device(){};
			Device(){};

			/**
			 * Get the width, in pixels, of the displays resolution.
			 */
			virtual uint32_t getDisplayResolutionWidth() = 0;

			/**
			 * Get the height, in pixels, of the displays resolution.
			 */
			virtual uint32_t getDisplayResolutionHeight() = 0;

		protected:

		private:
	};
}

#endif