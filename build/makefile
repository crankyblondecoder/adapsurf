# libAdapSurf Root Makefile
# -------------------------

export objFiles := DrmDevice.o Exception.o Framebuffer.o Surface.o

# NOTE: Put required libs last in the build so Debian based systems don't cry.
export libs = drm

export debugLibName = libAdapSurf_debug.so.0.1
export releaseLibName = libAdapSurf.so.0.1

.PHONY : debug
debug :
	$(MAKE) -C debug debug

.PHONY : clean
clean :
	- $(MAKE) -C debug clean
	- $(MAKE) -C test clean
	- $(MAKE) -C release clean
	- rm $(debugLibName)
	- rm $(releaseLibName)

.PHONY : test
test : debug
	$(MAKE) -C test test

.PHONY : runtest
runtest :
	$(MAKE) -C test runtest

.PHONY : release
release :
	$(MAKE) -C release release