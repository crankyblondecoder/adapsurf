# libAdapSurf Root Makefile
# -------------------------

export objFiles := CairoSurface.o DrmDevice.o DrmFramebuffer.o Exception.o Framebuffer.o Surface.o

# NOTE: Put required libs last in the build so Debian based systems don't cry.
export libs = drm cairo

export debugLibName = libAdapSurf_debug.so.0.1
export debugLibNameStatic = libAdapSurf_debug.a
export releaseLibName = libAdapSurf.so.0.1
export releaseLibNameStatic = libAdapSurf.a

.PHONY : debug
debug :
	cd debug; $(MAKE) --makefile=../makefile_debug debug

.PHONY : clean
clean :
	- cd debug; $(MAKE) --makefile=../makefile_debug clean
	- cd test; $(MAKE) --makefile=../makefile_test clean
	- cd release; $(MAKE) --makefile=../makefile_release clean
	- rm $(debugLibName)
	- rm $(debugLibNameStatic)
	- rm $(releaseLibName)
	- rm $(releaseLibNameStatic)

.PHONY : test
test : debug
	cd test; $(MAKE) --makefile=../makefile_test test

.PHONY : runtest
runtest :
	cd test; $(MAKE) --makefile=../makefile_test runtest

.PHONY : release
release :
	cd release; $(MAKE) --makefile=../makefile_release release

.PHONY : cardinfo
cardinfo : debug
	cd test; $(MAKE) --makefile=../makefile_test cardinfo
