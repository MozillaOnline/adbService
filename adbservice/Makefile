CXX = gcc
INCLUDEDIR = -I./ -I/usr/include
LIBDIR = -ladbservice 
LDFLAGS = -L./ -L/usr/lib
CXXFLAGS = -g -O -Wall
all : libadbservice.so test_so
.PHONY : all
libadbservice.so:adbservice.o
	$(CXX) $(LDFLAGS) $(INCLUDEDIR) -fPIC -shared adbservice.c -o $@
test_so:test_so.o
	$(CXX) $(LDFLAGS) $(INCLUDEDIR) $(LIBDIR) test_so.c -o $@
clean:
	rm -f  *.a *.o *.so *.log test_so 
