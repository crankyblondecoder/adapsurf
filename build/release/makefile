# libSNL Source Directory Debug Makefile
# -------------------------------------

# Tell make to search other paths. Separate multiple with ":"
VPATH = ../../src/

cname = g++
cflags = -Wall -fPIC -O3

CXX = $(cname)
CXXFLAGS = $(cflags)

.PHONY : release
release : $(objFiles)
	@echo
	@echo "*** Building - Release ***"
	@echo
	$(cname) $(cflags) -shared -o ../$(releaseLibName) $(objFiles)
	@echo
	@echo

# GNU Make will automatically update included files.
ifneq ($(MAKECMDGOALS),clean)
    include $(objFiles:.o=.dep)
endif

%.dep : %.cpp %.h
	$(cname) -MM $< > $@

.PHONY : clean
clean :
	rm *.o *.dep $(libName)
