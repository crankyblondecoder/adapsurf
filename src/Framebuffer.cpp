#include "Framebuffer.hpp"

using namespace adapsurf;

Framebuffer::~Framebuffer()
{
}

Framebuffer::Framebuffer() {

	_width = 0;
	_height = 0;
	_bpp = 0;
	_stride = 0;
	_pixelRatio = 0;
	_buffer = 0;
	_bufferSize = 0;
}