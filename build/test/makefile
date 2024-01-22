# libSNL Test Directory Makefile
# ------------------------------

# Tell make to search other paths. Separate multiple with ":"
VPATH = ../../src/ : ../../src/test/

sourceLocn = ../../src/test/

cname = g++
cflags = -Wall -fPIC -g

CXX = $(cname)
CXXFLAGS = $(cflags)

.PHONY : test
test : snlUnitTest.o snlTest.cpp
	@echo
	@echo "*** Building - Test ***"
	@echo
	$(cname) $(cflags) -o snlTest $(sourceLocn)snlTest.cpp snlUnitTest.o $(addprefix ../debug/, $(objFiles))
	@echo
	@echo

snlUnitTest.o : snlUnitTest.cpp snlUnitTest.hpp

.PHONY : clean
clean :
	rm *.o *.dep snlTest

.PHONY : runtest
runtest : test
	@echo
	@echo "*** Running - Test ***"
	@echo
	./snlTest
	@echo
	@echo
