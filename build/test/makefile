# libAdapSurf Test Directory Makefile
# -----------------------------------

# NOTE: Put required libs last in the build so Debian based systems don't cry.

# Tell make to search other paths. Separate multiple with ":"
VPATH = ../../src/ : ../../src/test/

sourceLocn = ../../src/test/

cname = g++
cflags = -Wall -fPIC -g

CXX = $(cname)
CXXFLAGS = $(cflags)

.PHONY : test
test : adsUnitTest.o adsTest.cpp
	@echo
	@echo "*** Building - Test ***"
	@echo
	$(cname) $(cflags) -o adsTest $(sourceLocn)adsTest.cpp adsUnitTest.o $(addprefix ../debug/, $(objFiles)) $(addprefix -l, $(libs))
	@echo
	@echo

adsUnitTest.o : adsUnitTest.cpp adsUnitTest.hpp

.PHONY : clean
clean :
	rm *.o *.dep adsTest cardinfo

.PHONY : runtest
runtest : test
	@echo
	@echo "*** Running - Test ***"
	@echo
	./adsTest
	@echo
	@echo

cardinfo : cardinfo.cpp
	@echo
	@echo "*** Building - Card Info ***"
	@echo
	$(cname) $(cflags) -o cardinfo $(sourceLocn)cardinfo.cpp $(addprefix ../debug/, $(objFiles)) $(addprefix -l, $(libs))
	@echo
	@echo