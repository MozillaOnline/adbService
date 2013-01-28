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
    int (*findDevice)() = dlsym(handler, "findDevice");
    assert(findDevice != NULL);
    int (*setupDevice)() = dlsym(handler, "setupDevice");
    assert(setupDevice != NULL);
    printf("test so findDevice=%d\n", (*findDevice)());
    printf("test so setupDevice=%d\n", (*setupDevice)());
    dlclose(handler);
    return 0;
}
