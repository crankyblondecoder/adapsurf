#ifndef ADS_CAIRO_SURFACE_H
#define ADS_CAIRO_SURFACE_H

#include <cstdint>

#include <cairo/cairo.h>

#include "Surface.hpp"

namespace adapsurf {

	/**
	 * Cairo based surface.
	 */
	class CairoSurface : public Surface
	{
		public:

			virtual ~CairoSurface();

			/**
			 * Create a new cairo based surface.
			 * @param localPositionX Position of surface in local coordinates. X coordinate. Positive X is right.
			 * @param localPositionY Position of surface in local coordinates. Y coordinate. Positive Y is down.
			 * @param width Width of surface.
			 * @param height Height of surface.
			 */
			CairoSurface(int localPositionX, int localPositionY, int width, int height);

			// Impl.
			virtual uint8_t* getPixelData();

			// Impl.
			virtual unsigned getPixelDataStride();

			// Impl.
			virtual unsigned getWidth() = 0;

			// Impl.
			virtual unsigned getHeight() = 0;

		protected:

		private:

			/** Cairo surface to render to. */
			cairo_surface_t* _cSurface;
	};
}

#endif