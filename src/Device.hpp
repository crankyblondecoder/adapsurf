#ifndef ADS_DEVICE_H
#define ADS_DEVICE_H

#include <cstdint>

#include "Framebuffer.hpp"

namespace adapsurf
{
	/**
	 * A device abstracts a specific display device and provides a framebuffer into which drawing can take place.
	 */
	class Device
	{
		public:

			virtual ~Device(){};
			Device();

			/**
			 * Get the width, in pixels, of the displays resolution.
			 */
			virtual uint32_t getDisplayResolutionWidth() = 0;

			/**
			 * Get the height, in pixels, of the displays resolution.
			 */
			virtual uint32_t getDisplayResolutionHeight() = 0;

			/**
			 * Get the framebuffer to draw to. This will not be cleared ie It will still have its previous pixel data.
			 */
			virtual Framebuffer* getDrawToFramebuffer() = 0;

			/**
			 * Set the clear colour for all, draw to, frame buffers.
			 * Each framebuffer will be cleared just prior to becoming the current back buffer to draw to.
			*/
			virtual void setClearColour(double red, double green, double blue);

		protected:

			/** Get whether the clear colour has been set. */
			bool _getClearColourSet();

			/** Get the red component of the clear colour. */
			double _getClearColourRed();

			/** Get the green component of the clear colour. */
			double _getClearColourGreen();

			/** Get the blue component of the clear colour. */
			double _getClearColourBlue();

		private:

			/** Whether the clear colour was explicitly set. */
			bool _clearColourSet;

			/** The clear colour red component. */
			double _clearColourRed;

			/** The clear colour green component. */
			double _clearColourGreen;

			/** The clear colour blue component. */
			double _clearColourBlue;
	};
}

#endif