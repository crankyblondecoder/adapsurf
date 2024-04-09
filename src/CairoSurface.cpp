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
	if(_cSurface)
	{
		cairo_surface_flush(_cSurface);
		return cairo_image_surface_get_data(_cSurface);
	}

	return 0;
}

unsigned CairoSurface::_getPixelDataStride()
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

cairo_t* CairoSurface::_getContext()
{
	return _cairoContext;
}

void CairoSurface::clear(double red, double green, double blue, double alpha)
{
	if(_cairoContext)
	{
		cairo_set_source_rgba(_cairoContext, red, green, blue, alpha);
		cairo_paint(_cairoContext);
	}
}

void CairoSurface::compose(Surface& surface)
{
	TODO;
}