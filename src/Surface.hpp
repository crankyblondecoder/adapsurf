#ifndef ADS_SURFACE_H
#define ADS_SURFACE_H

namespace adapsurf {

	/**
	 * Base class of all surfaces.
	 * @note For now only Cairo's CAIRO_FORMAT_ARGB32 format is supported. Internally it can be whatever but it must be able
	 *       to return its buffer in this format.
	 */
	class Surface
	{
		public:

			~Surface();
			Surface();

			/**
			 * Get this surfaces pixel data.
			 * This will include the composited pixel data of all child surfaces.
			 * @returns This surfaces pixel data in CAIRO_FORMAT_ARGB32 format.
			 */
			virtual uint8_t* getPixelData() = 0;

			/**
			 * Get the size of this surfaces pixel data.
			 * @returns Size in bytes.
			*/
			virtual unsigned getPixelDataSize() = 0;

			/**
			 * Get the stride of this surfaces pixel data.
			 * This is essentially the number of bytes per "scan line" and may be larger than the number required for the pixel
			 * width of this surface.
			 * @returns Stride, in bytes.
			 */
			virtual unsigned getPixelDataStride() = 0;

			/**
			 * Get the position of this surface relative to it's parent. X coordinate. Positive is right.
			 * @return X coordinate in pixels.
			 */
			unsigned getPositionX();

			/**
			 * Get the position of this surface relative to it's parent. Y coordinate. Positive is down.
			 * @returns Y coordinate in pixels.
			 */
			unsigned getPositionY();

			/**
			 * Get the current width this surface, in pixels.
			 */
			unsigned getWidth();

			/**
			 * Get the current height of this surface, in pixels.
			 */
			unsigned getHeight();

		protected:

			void _setParent(Surface* parent);

		private:

			/** Parent of this surface. Any part of this surface which goes outside the parent is clipped. */
			Surface* __parent;

			/** Child surfaces of this surface. */
			Surface** __children;

			/** Position of this surface relative to its parent. X coordinate. Positive X is right. */
			unsigned __positionX;
			/** Position of this surface relative to its parent. Y coordinate. Positive Y is down. */
			unsigned __positionY;

			/** Minimum width of surface in pixels. */
			unsigned __minWidth;
			/** Minimum height of surface in pixels. */
			unsigned __minHeight;

			/** Maximum width of surface in pixels. */
			unsigned __maxWidth;
			/** Maximum height of surface in pixels. */
			unsigned __maxHeight;

			/** Preferred width of surface in pixels. */
			unsigned __prefWidth;
			/** Preferred height of surface in pixels. */
			unsigned __prefHeight;

			/** Current width of surface in pixels. */
			unsigned __curWidth;
			/** Current height of surface in pixels. */
			unsigned __curHeight;
	};
}

#endif