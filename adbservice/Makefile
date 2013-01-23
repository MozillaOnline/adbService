CXX = gcc
INCLUDEDIR = -I./ 
LDFLAGS = -L./

TESTINCLUDEDIR = -I/usr/include
TESTLDFLAGS = -L/usr/lib
TESTLIBDIR = -ldl

all : libadbservice.so test_so
.PHONY : all
libadbservice.so:adbservice.o
	$(CXX) -o $@ adbservice.c $(LDFLAGS) $(INCLUDEDIR) -fPIC -shared
test_so:test_so.o
	$(CXX) -o $@ test_so.c $(TESTLIBDIR)
clean:
	rm -f  *.a *.o *.so *.log test_so 
