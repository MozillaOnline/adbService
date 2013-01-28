CXX = gcc
TESTLIBDIR = -ldl

all : test_so
.PHONY : all
test_so:
	$(CXX) -o $@ test_so.c $(TESTLIBDIR)
clean:
	rm -f  test_so 
