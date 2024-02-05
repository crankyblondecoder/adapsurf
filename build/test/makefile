# libAdapSurf Test Directory Makefile
# -----------------------------------

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
	$(cname) $(cflags) -o adsTest $(sourceLocn)adsTest.cpp adsUnitTest.o $(addprefix ../debug/, $(objFiles)) -l$(libs)
	@echo
	@echo

adsUnitTest.o : adsUnitTest.cpp adsUnitTest.hpp

.PHONY : clean
clean :
	rm *.o *.dep adsTest

.PHONY : runtest
runtest : test
	@echo
	@echo "*** Running - Test ***"
	@echo
	./adsTest
	@echo
	@echo
