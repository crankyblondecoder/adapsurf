#ifndef ADS_SURFACE_H
#define ADS_SURFACE_H

namespace adapsurf {

	class surface
	{
		public:

			~surface();
			surface();

		private:

			/** Minimum width of surface in pixels. */
			double _minWidth;
			/** Minimum height of surface in pixels. */
			double _minHeight;

			/** Maximum width of surface in pixels. */
			double _maxWidth;
			/** Maximum height of surface in pixels. */
			double _maxHeight;

			/** Preferred width of surface in pixels. */
			double _prefWidth;
			/** Preferred height of surface in pixels. */
			double _prefHeight;

			/** Current width of surface in pixels. */
			double _curWidth;
			/** Current height of surface in pixels. */
			double _curHeight;
	};
}

#endif