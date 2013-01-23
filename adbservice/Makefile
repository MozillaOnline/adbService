CXX = gcc
INCLUDEDIR = -I./ 
LDFLAGS = -L./
XP_LINUX=
TESTINCLUDEDIR = -I/usr/include
TESTLDFLAGS = -L/usr/lib
TESTLIBDIR = -ldl

all : libadbservice.so test_so
.PHONY : all
libadbservice.so:
	$(CXX) -o $@ adbservice.c $(LDFLAGS) $(INCLUDEDIR) -fPIC -shared -D XP_LINUX

test_so:
	$(CXX) -o $@ test_so.c $(TESTLIBDIR)
clean:
	rm -f  *.a *.o *.so *.log test_so 
