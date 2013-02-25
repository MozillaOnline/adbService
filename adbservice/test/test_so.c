#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dlfcn.h> /* 必须加这个头文件 */
#include <assert.h>

int main(int argc, char **argv)
{
    void *handler = dlopen("./libadbservice.so", RTLD_NOW);
    assert(handler != NULL);
    int (*setupPath)() = dlsym(handler, "setupPath");
    int (*findDevice)() = dlsym(handler, "findDevice");
    int (*setupDevice)() = dlsym(handler, "setupDevice");
	int (*pullfile)() = dlsym(handler, "pullfile");
	int (*pushfile)() = dlsym(handler, "pushfile");

    printf("test so setupPath\n", (*setupPath)("../linux/adb"));
    
    printf("test so findDevice=%d\n", (*findDevice)());
    printf("test so setupDevice=%d\n", (*setupDevice)());
    
    printf("test so pullfile=%d\n", (*pullfile)("/system/b2g/libplc4.so", "./"));
    printf("test so pushfile=%d\n", (*pushfile)("./libplc4.so", "/system/b2g/"));
    
    dlclose(handler);
    return 0;
}
