#include "CairoSurface.hpp"
#include "Exception.hpp"

using namespace adapsurf;

CairoSurface::~CairoSurface()
{
	if(_cSurface) cairo_surface_destroy(_cSurface);
	if(_cairoContext) cairo_destroy(_cairoContext);
}

CairoSurface::CairoSurface(int localPositionX, int localPositionY, int width, int height) :
	Surface(localPositionX, localPositionY)
{
	_cSurface = cairo_image_surface_create(cairo_format_t::CAIRO_FORMAT_ARGB32, width, height);

	if(!_cSurface)
	{
		std::string msg("Could not create a valid cairo surface.");
		throw Exception(Exception::Error::SURF_COULD_NOT_CREATE, msg);
	}

	_cairoContext = cairo_create(_cSurface);

	if(cairo_status(_cairoContext) == cairo_status_t::CAIRO_STATUS_NO_MEMORY)
	{
		cairo_surface_destroy(_cSurface);

		_cairoContext = 0;

		std::string msg("Could not create a cairo context. No memory.");
		throw Exception(Exception::Error::SURF_COULD_NOT_CREATE, msg);
	}
}

uint8_t* CairoSurface::_getPixelData()
{
	return __getPixelData();
}

uint8_t* CairoSurface::__getPixelData()
{
	if(_cSurface)
	{
		cairo_surface_flush(_cSurface);
		return cairo_image_surface_get_data(_cSurface);
	}

	return 0;
}

unsigned CairoSurface::_getPixelDataStride()
{
	return __getPixelDataStride();
}

unsigned CairoSurface::__getPixelDataStride()
{
	if(_cSurface)
	{
		return cairo_image_surface_get_stride(_cSurface);
	}

	return 0;
}

unsigned CairoSurface::getWidth()
{
	return _getWidth();
}

unsigned CairoSurface::_getWidth()
{
	if(_cSurface) {

		return cairo_image_surface_get_width(_cSurface);
	}

	return 0;
}

unsigned CairoSurface::getHeight()
{
	return _getHeight();
}

unsigned CairoSurface::_getHeight()
{
	if(_cSurface) {

		return cairo_image_surface_get_height(_cSurface);
	}

	return 0;
}

cairo_t* CairoSurface::getContext()
{
	return _cairoContext;
}

cairo_t* CairoSurface::_getContext()
{
	return _cairoContext;
}

void CairoSurface::clear(double red, double green, double blue, double alpha)
{
	if(_cairoContext)
	{
		cairo_save(_cairoContext);
		cairo_set_source_rgba(_cairoContext, red, green, blue, alpha);
		cairo_set_operator(_cairoContext, CAIRO_OPERATOR_SOURCE);
		cairo_paint(_cairoContext);
		cairo_restore(_cairoContext);
	}
}

void CairoSurface::compose(Surface& surface)
{
	int srcPosnX = surface.getGlobalPositionX();
	int srcPosnY = surface.getGlobalPositionY();

	unsigned srcWidth = surface.getWidth();
	unsigned srcHeight = surface.getHeight();

	// Source left and bottom extremities.
	unsigned srcEndX = srcPosnX + srcWidth - 1;
	unsigned srcEndY = srcPosnY + srcHeight - 1;

	// Destination is this surface, being written to.
	unsigned destWidth = _getWidth();
	unsigned destHeight = _getHeight();

	// Short circuit the composition if none of the source surface is within this framebuffer.
	if(srcPosnX >= (int)destWidth || srcPosnY >= (int)destHeight || srcEndX < 0 || srcEndY < 0) return;

	uint8_t* srcPixelData = surface.getPixelData();
	uint8_t* destPixelData = __getPixelData();

	if(srcPixelData)
	{
		//    |---|-------------|---|

		unsigned srcStride = surface.getPixelDataStride();
		unsigned destStride = __getPixelDataStride();

		// Source line index to start copying from.
		unsigned srcStartLine = srcPosnY < 0 ? -srcPosnY : 0;

		// Source line index to finish copying from.
		unsigned srcEndLine = srcHeight - 1;
		if(srcEndY >= destHeight) srcEndLine -= srcEndY - destHeight + 1;

		// Number of source lines to be copied.
		unsigned numLines = srcEndLine - srcStartLine + 1;

		// Start offset in source line to copy from. In pixels.
		unsigned srcLineStartOffset = srcPosnX < 0 ? -srcPosnX : 0;

		// End offset (inclusive) of source line to copy from. In pixels.
		unsigned srcLineEndOffset = srcWidth - 1;
		if(srcEndX >= destWidth) srcLineEndOffset -= srcEndX - destWidth + 1;

		// Number of pixels to copy from the source per line.
		unsigned numPixCopyPerLine = srcLineEndOffset - srcLineStartOffset + 1;

		// Number of source bytes to go from end of line to start of next line (with offset).
		unsigned nextSrcLineStride = srcStride - numPixCopyPerLine * 4;

		// Number of destination bytes to go from end of line to start of next line (with offset).
		unsigned nextDestLineStride = destStride - numPixCopyPerLine * 4;

		// Calculate initial source copy from position.
		uint8_t* srcCpPosn = srcPixelData + srcStartLine * srcStride + srcLineStartOffset * 4;

		// Calculate initial destination copy to position.
		uint8_t* destCpPosn = destPixelData + (srcPosnY + srcStartLine) * destStride + (srcPosnX + srcLineStartOffset) * 4;

		// Copy to destination while taking into account the pixel data has pre-multiplied alpha.

		// Current source pixel values.
		uint8_t srcBlue;
		uint8_t srcGreen;
		uint8_t srcRed;
		uint8_t srcAlpha;

		// Effective alpha value used to scale current destination pixel channels.
		double effAlpha;

		for(unsigned line = 0; line < numLines; line++)
		{
			for(unsigned pixel = 0; pixel < numPixCopyPerLine; pixel++)
			{
				// Work at byte level because it then becomes machine endien agnostic.
				srcBlue = *srcCpPosn++;
				srcGreen = *srcCpPosn++;
				srcRed = *srcCpPosn++;
				srcAlpha = *srcCpPosn++;

				// No point in processing pixel if it is completely transparent.
				if(srcAlpha > 0)
				{
					if(srcAlpha == 0xFF)
					{
						// Just straight copy is required.

						*destCpPosn = srcBlue;
						destCpPosn++;
						*destCpPosn = srcGreen;
						destCpPosn++;
						*destCpPosn = srcRed;
						destCpPosn++;
						*destCpPosn = srcAlpha;
						destCpPosn++;
					}
					else
					{
						// Alpha blend.
						effAlpha = 1.0 - ((double)srcAlpha / 255.0);

						*destCpPosn = srcBlue + (uint8_t)((double)(*destCpPosn) * effAlpha);
						destCpPosn++;
						*destCpPosn = srcGreen + (uint8_t)((double)(*destCpPosn) * effAlpha);
						destCpPosn++;
						*destCpPosn = srcRed + (uint8_t)((double)(*destCpPosn) * effAlpha);
						destCpPosn++;
						*destCpPosn = srcAlpha + (uint8_t)((double)(*destCpPosn) * effAlpha);
						destCpPosn++;
					}
				}
				else
				{
					destCpPosn += 4;
				}
			}

			srcCpPosn += nextSrcLineStride;
			destCpPosn += nextDestLineStride;
		}
	}
}