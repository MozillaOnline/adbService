#include <stdlib.h>
#include <stdio.h>

#ifndef XP_LINUX
#include <windows.h>
#endif

#define BUFFER_SIZE 1024 
#define CMD_SIZE 128
#define LOCAL_PORT 10010
#define REMOTE_PORT 10010

#ifdef XP_WIN
__declspec(dllexport) 
#endif
int findDevice()
{
	char *logname ="adbfind.log"; 
	char *sigstr = "List of devices attached";
	char *devstr = "device";
	FILE *fp = 0;
	int numread = 0;
	char buffer[BUFFER_SIZE] = {0};
	char *pb = 0;
	char cmd[CMD_SIZE] = {0};
	int ret = 0;
	
#ifdef XP_WIN


#else
	#ifdef XP_MAC
		strcpy(cmd, "./adb-mac devices > "); 
		strcat(cmd, logname); 
		ret = system(cmd);
	#else
		strcpy(cmd, "./adb-linux devices > "); 
		strcat(cmd, logname); 
		ret = system(cmd);
	#endif
#endif
	if(ret)
		return 0;
	if( (fp = fopen(logname,"r")) == NULL )
	{
		printf( "The file was not opened\n");
		return 0;
	}
	while( (numread = fread(buffer, sizeof(char), BUFFER_SIZE, fp)))
	{
		if(numread==0)
			break;
	}
	fclose(fp);
	buffer[strlen(buffer)]='\0';
	pb = buffer;
	if(strncmp(pb, sigstr,strlen(sigstr)))
		return 0;
	pb = pb + strlen(sigstr);
	if(!strstr(pb, devstr))
		return 0;
	printf( "The result is %s\n", pb);
	return 1;
}
#ifdef XP_WIN
__declspec(dllexport) 
#endif
int setupDevice()
{
    char cmd[CMD_SIZE] = {0};
	int ret = 0;
	
#ifdef XP_WIN


#else
	#ifdef XP_MAC
		sprintf(cmd, "./adb-mac forward tcp:%d tcp:%d", LOCAL_PORT,REMOTE_PORT);
	#else
		sprintf(cmd, "./adb-linux forward tcp:%d tcp:%d", LOCAL_PORT,REMOTE_PORT);
	#endif
	ret = system(cmd);
#endif

	if(ret)
		return 0;
    return 1;
}
