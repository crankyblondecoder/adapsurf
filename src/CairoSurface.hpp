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
			virtual unsigned getWidth();

			// Impl.
			virtual unsigned getHeight();

			/**
			 * Clear this surface.
			 * @param red Red component.
			 * @param green Green component.
			 * @param blue Blue component.
			 */
			void clear(double red, double green, double blue);

		protected:

			/** Get the cairo context which is used to draw to the surface. */
			cairo_t* _getContext();

			unsigned _getWidth();

			unsigned _getHeight();

		private:

			/** Cairo surface to render to. */
			cairo_surface_t* _cSurface;

			/** Cairo context. */
			cairo_t* _cairoContext;
	};
}

#endif