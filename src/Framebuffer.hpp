#ifndef ADS_FRAMEBUFFER_H
#define ADS_FRAMEBUFFER_H

#include <cstdint>

namespace adapsurf {

	/**
	 * A framebuffer that all surfaces are rendered to.
	 * @note Currently this must have a stride size (number of bytes between rows) that is compatible with Cairo.
	 */
	class Framebuffer
	{
		public:

			virtual ~Framebuffer();

			Framebuffer();

		protected:

		private:

			/** The ratio of the width to height (width / height) of the pixels the framebuffer displays. */
			float _pixelRatio;
	};
}

#endif