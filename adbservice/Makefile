CXX = gcc
INCLUDEDIR = -I./ 
LDFLAGS = -L./

all : libadbservice.so
.PHONY : all
libadbservice.so:
	$(CXX) -o $@ adbservice.c $(LDFLAGS) $(INCLUDEDIR) -fPIC -shared -D XP_LINUX
clean:
	rm -f  *.a *.o *.so 
