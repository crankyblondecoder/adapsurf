#ifndef ADS_SURFACE_H
#define ADS_SURFACE_H

#define CHILD_ARRAY_PAGE_SIZE 8

namespace adapsurf {

	class Framebuffer;

	/**
	 * Base class of all surfaces.
	 * @note For now only Cairo's CAIRO_FORMAT_ARGB32 format is supported for pixel data. Internally it can be whatever but
	 * it must be able to return its buffer in this format.
	 */
	class Surface
	{
		public:

			virtual ~Surface();
			Surface();

			/**
			 * Compose this surface and all its children onto a framebuffer.
			 * @param framebuffer Framebuffer to compose this surface onto.
			 */
			void compose(Framebuffer& framebuffer);

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
			 * Get the global position of this surface. X coordinate. Positive is right.
			 * @return X coordinate in pixels.
			 */
			int getGlobalPositionX();

			/**
			 * Get the global position of this surface. Y coordinate. Positive is down.
			 * @returns Y coordinate in pixels.
			 */
			int getGlobalPositionY();

			/**
			 * Get the current width this surface, in pixels.
			 */
			unsigned getWidth();

			/**
			 * Get the current height of this surface, in pixels.
			 */
			unsigned getHeight();

			/** Add a child surface to this surface. */
			void addChild(Surface* child);

			/** Remove a child surface from this surface. */
			void removeChild(Surface* child);

		protected:

		private:

			/** Parent of this surface. Any part of this surface which goes outside the parent is clipped. */
			Surface* __parent;

			/** Child surfaces of this surface. */
			Surface** __children;
			/** Size of the children array. */
			unsigned __childrenSize;
			/** Number of children present. */
			unsigned __numChildren;

			/** Position (local) of this surface relative to its parent. X coordinate. Positive X is right. */
			int __localPositionX;
			/** Position (local) of this surface relative to its parent. Y coordinate. Positive Y is down. */
			int __localPositionY;

			/** Position of this surface in global (absolute) coordinates. X coordinate. */
			int __globalPositionX;
			/** Position of this surface in global (absolute) coordinates. Y coordinate. */
			int __globalPositionY;

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

			/** Remove a child surface from this surface. */
			void __removeChild(Surface* child);

			/**
			 * Set the parent of this surface.
			 * Does _not_ remove this from existing parent. That must be done explicitly.
			 */
			void __setParent(Surface* parent);
	};
}

#endif