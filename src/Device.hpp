#ifndef ADS_DEVICE_H
#define ADS_DEVICE_H

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

		protected:

		private:
	};
}

#endif