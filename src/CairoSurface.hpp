#ifndef ADS_CAIRO_SURFACE_H
#define ADS_CAIRO_SURFACE_H

#include <cairo/cairo.h>

namespace adapsurf {

	/**
	 * Cairo based surface.
	 */
	class CairoSurface
	{
		public:

			virtual ~CairoSurface();
			CairoSurface();

		protected:

		private:

			/** Cairo surface to render to. */
			cairo_surface_t* _cSurface;
	};
}

#endif