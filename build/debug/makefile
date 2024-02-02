# libAdapSurf Source Directory Debug Makefile
# -------------------------------------------

# Tell make to search other paths. Separate multiple with ":"
VPATH = ../../src/

cname = g++
cflags = -Wall -fPIC -g -Og

CXX = $(cname)
CXXFLAGS = $(cflags)

.PHONY : debug
debug : $(objFiles)
	@echo
	@echo "*** Building - Debug ***"
	@echo
	$(cname) $(cflags) -l$(libs) -shared -o ../$(debugLibName) $(objFiles)
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
