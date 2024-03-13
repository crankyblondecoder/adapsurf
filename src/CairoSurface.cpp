#include "CairoSurface.hpp"
#include "Exception.hpp"

using namespace adapsurf;

CairoSurface::~CairoSurface()
{
	if(_cSurface) cairo_surface_destroy(_cSurface);
	if(cairoContext) cairo_destroy(cairoContext);
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

	cairoContext = cairo_create(_cSurface);

	if(cairo_status(cairoContext) == cairo_status_t::CAIRO_STATUS_NO_MEMORY)
	{
		cairo_surface_destroy(_cSurface);

		cairoContext = 0;

		std::string msg("Could not create a cairo context. No memory.");
		throw Exception(Exception::Error::SURF_COULD_NOT_CREATE, msg);
	}
}

uint8_t* CairoSurface::getPixelData()
{
	if(_cSurface)
	{
		cairo_surface_flush(_cSurface);
		return cairo_image_surface_get_data(_cSurface);
	}

	return 0;
}

unsigned CairoSurface::getPixelDataStride()
{
	if(_cSurface)
	{
		return cairo_image_surface_get_stride(_cSurface);
	}

	return 0;
}

unsigned CairoSurface::getWidth()
{
	if(_cSurface) {

		return cairo_image_surface_get_width(_cSurface);
	}

	return 0;
}

unsigned CairoSurface::getHeight()
{
	if(_cSurface) {

		return cairo_image_surface_get_height(_cSurface);
	}

	return 0;
}
