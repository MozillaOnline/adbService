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


    printf("test so setupPath\n", (*setupPath)("../linux/adb"));
    printf("test so findDevice=%d\n", (*findDevice)());
    printf("test so setupDevice=%d\n", (*setupDevice)());
    dlclose(handler);
    return 0;
}
