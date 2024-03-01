#ifndef GRID_ADS_SURFACE_H
#define GRID_ADS_SURFACE_H

#include "Surface.hpp"

namespace adapsurf {

	/**
	 * Surface that only lays out children in a grid and doesn't draw anything.
	 * @note This is yet to be implemented.
	 */
	class GridSurface : public Surface
	{
		public:

			virtual ~GridSurface();
			GridSurface();
	};
}

#endif