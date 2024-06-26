#ifndef ADS_CAIRO_SURFACE_H
#define ADS_CAIRO_SURFACE_H

#include <cstdint>

#include <cairo/cairo.h>

#include "Surface.hpp"

namespace adapsurf
{
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
			virtual unsigned getWidth();

			// Impl.
			virtual unsigned getHeight();

			/**
			 * Clear this surface.
			 * @param red Red component.
			 * @param green Green component.
			 * @param blue Blue component.
			 * @param alpha Alpha component.
			 */
			void clear(double red, double green, double blue, double alpha);

			// Impl.
			virtual void compose(Surface& surface);

			/** Get the cairo context which is used to draw to the surface. */
			cairo_t* getContext();

		protected:

			/** Get the cairo context which is used to draw to the surface. */
			cairo_t* _getContext();

			unsigned _getWidth();

			unsigned _getHeight();

			// Impl.
			virtual uint8_t* _getPixelData();

			// Impl.
			virtual unsigned _getPixelDataStride();

		private:

			/** Cairo surface to render to. */
			cairo_surface_t* _cSurface;

			/** Cairo context. */
			cairo_t* _cairoContext;

			/** @see getPixelDataStride() */
			unsigned __getPixelDataStride();

			/** @see getPixelData() */
			uint8_t* __getPixelData();
	};
}

#endif