#ifndef ADS_SURFACE_H
#define ADS_SURFACE_H

#include <cstdint>

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

			/**
			 * Construct new surface.
			 * @note If this surface doesn't have a parent local coordinates are equivalent to global coordinates.
			 * @param localPositionX Position of surface in local coordinates. X coordinate. Positive X is right.
			 * @param localPositionY Position of surface in local coordinates. Y coordinate. Positive Y is down.
			 */
			Surface(int localPositionX, int localPositionY);

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
			virtual unsigned getWidth() = 0;

			/**
			 * Get the current height of this surface, in pixels.
			 */
			virtual unsigned getHeight() = 0;

			/** Add a child surface to this surface. */
			void addChild(Surface* child);

			/** Remove a child surface from this surface. */
			void removeChild(Surface* child);

		protected:

			/**
			 * Get the global position of this surface. X coordinate. Positive is right.
			 * @return X coordinate in pixels.
			 */
			int _getGlobalPositionX();

			/**
			 * Get the global position of this surface. Y coordinate. Positive is down.
			 * @returns Y coordinate in pixels.
			 */
			int _getGlobalPositionY();

		private:

			/** Parent of this surface. Any part of this surface which goes outside the parent is clipped. */
			Surface* _parent;

			/** Child surfaces of this surface. */
			Surface** _children;
			/** Size of the children array. */
			unsigned _childrenSize;
			/** Number of children present. */
			unsigned _numChildren;

			/** Position (local) of this surface relative to its parent. X coordinate. Positive X is right. */
			int _localPositionX;
			/** Position (local) of this surface relative to its parent. Y coordinate. Positive Y is down. */
			int _localPositionY;

			/** Position of this surface in global (absolute) coordinates. X coordinate. */
			int _globalPositionX;
			/** Position of this surface in global (absolute) coordinates. Y coordinate. */
			int _globalPositionY;

			/** Remove a child surface from this surface. */
			void __removeChild(Surface* child);

			/**
			 * Set the parent of this surface.
			 * Does _not_ remove this from existing parent. That must be done explicitly.
			 */
			void __setParent(Surface* parent);

			/**
			 * Update the global position of this surface based on its parents global position.
			 */
			void __updateGlobalPosn();
	};
}

#endif