# libAdapSurf Source Directory Debug Makefile
# -------------------------------------------

# NOTE: Put required libs last in the build so Debian based systems don't cry.

# Tell make to search other paths. Separate multiple with ":"
VPATH = ../../src/

cname = g++
cflags = -Wall -fPIC -g3 -Og

CXX = $(cname)
CXXFLAGS = $(cflags)

.PHONY : debug
debug : $(objFiles)
	@echo
	@echo "*** Building - Debug ***"
	@echo
	$(cname) $(cflags) -shared -o ../$(debugLibName) $(objFiles) $(addprefix -l, $(libs))
	@echo
	@echo

# GNU Make will automatically update included files.
ifneq ($(MAKECMDGOALS),clean)
    include $(objFiles:.o=.dep)
endif

%.dep : %.cpp %.hpp
	$(cname) -MM $< > $@

.PHONY : clean
clean :
	rm *.o *.dep
