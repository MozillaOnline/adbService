#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dlfcn.h> /* 必须加这个头文件 */
#include <assert.h>
#include <errno.h>

char chBuf[10240] = {0};
int main(int argc, char **argv)
{
    void *handler = dlopen("./libadbservice.so", RTLD_NOW);
    assert(handler != NULL);
    char* (*runCmd)() = dlsym(handler, "runCmd");

    printf("test so runCmd=%s\n", (*runCmd)("adb shell"));
    
    dlclose(handler);
/*
FILE *fp = 0;
        int numread = 0;
        memset(chBuf, 0, 10240);
                fp = popen ("cp Makefile Make1 2>&1", "r");
                if (fp == NULL)
                {
                        strcpy(chBuf,"error: popen.");
                        return chBuf;
                }

                while( (numread = fread(chBuf, sizeof(char), 10240, fp)))
                {
                        if(numread==0)
                                break;
                }
                pclose(fp);
	printf("test so xds4 %s\n", chBuf);
                return chBuf;
*/
    return 0;
}
