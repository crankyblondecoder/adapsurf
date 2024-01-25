#ifndef ADS_FRAMEBUFFER_H
#define ADS_FRAMEBUFFER_H

namespace adapsurf {

	/**
	 * A framebuffer that all surfaces are rendered to.
	 * @note Currently this must have a stride size (number of bytes between rows) that is compatible with Cairo.
	 */
	class framebuffer
	{
		public:

			~framebuffer();
			framebuffer();

		protected:

		private:

			/** Width of framebuffer in pixels. */
			unsigned _width;

			/** Height of framebuffer in pixels. */
			unsigned _height;

			/** Number of bits per pixel. */
			unsigned _bpp;

			/** Number of bytes between the start of the rows. This exists to support Cairo. */
			unsigned _stride;

			/** The ratio of the width to height (width / height) of the pixels the framebuffer displays. */
			float _pixelRatio;

			/** Total amount of memory that was allocated to the framebuffer. In bytes. */
			unsigned _sizeMemoryAlloc;
	};
}

#endif