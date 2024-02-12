#include "Framebuffer.hpp"

using namespace adapsurf;

Framebuffer::~Framebuffer()
{
}

Framebuffer::Framebuffer(unsigned width, unsigned height, unsigned bpp, unsigned stride, unsigned pixelRatio, uint8_t* buffer,
	unsigned bufferSize) : _width{width}, _height{height}, _bpp{bpp}, _stride{stride}, _buffer{buffer}, _bufferSize{bufferSize}
{

}