#include <stdlib.h>
#include <stdio.h>

#ifndef XP_LINUX
#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>


HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
 
BOOL CreateChildProcess(TCHAR *szCmdline); 
BOOL ReadFromPipe(CHAR *pszOutput); 
#endif

#define USB_NUM 16
#define BUFFER_SIZE 10240
#define CMD_SIZE 1024
#define LOCAL_PORT 10010
#define REMOTE_PORT 10010
char adbPath[BUFFER_SIZE] = {0};

#ifndef XP_LINUX
BOOL CreateChildProcess(TCHAR *szCmdline)
{ 
   PROCESS_INFORMATION piProcInfo; 
   STARTUPINFO siStartInfo;
   BOOL bSuccess = FALSE; 
 
   ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
   ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
   siStartInfo.cb = sizeof(STARTUPINFO); 
   siStartInfo.hStdError = g_hChildStd_OUT_Wr;
   siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
   siStartInfo.hStdInput = NULL;
   siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
    
   bSuccess = CreateProcess(NULL, 
      szCmdline,
      NULL,
      NULL,
      TRUE,
      0,
      NULL,
      NULL,
      &siStartInfo,
      &piProcInfo);
   if (bSuccess)  
   {
      CloseHandle(piProcInfo.hProcess);
      CloseHandle(piProcInfo.hThread);
   }
   return bSuccess;
}
 
BOOL ReadFromPipe(CHAR *pszOutput) 
{ 
   DWORD dwRead; 
   CHAR chBuf[BUFFER_SIZE] = {0}; 
   BOOL bSuccess = FALSE;
   HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
   CloseHandle(g_hChildStd_OUT_Wr);
   bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFFER_SIZE, &dwRead, NULL);
   if( ! bSuccess || dwRead == 0 ) 
	return 0;
	if(pszOutput != NULL)
		strcpy(pszOutput, chBuf);
    return 1;
} 
#endif

#ifndef XP_LINUX
__declspec(dllexport) 
#endif
char** findDevice()
{
	char *sigstr = "List of devices attached";
	char *devstr = "device";
	FILE *fp = 0;
	int numread = 0;
	char buffer[BUFFER_SIZE] = {0};
	char *pb = 0;
	char cmd[CMD_SIZE] = {0};
	char deviceslist[USB_NUM][CMD_SIZE] = {0};
	int ret = 0;
	
#ifndef XP_LINUX
	TCHAR szCmdline[CMD_SIZE]={0};
	SECURITY_ATTRIBUTES saAttr; 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 
	
	if(strlen(adbPath) > 0)
		sprintf(cmd, "%s devices", adbPath);
	else
		return NULL;
	
	MultiByteToWideChar(CP_ACP,0,cmd,strlen(cmd),szCmdline,CMD_SIZE); 
	
	if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
		return NULL;

	if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
		return NULL;
	if(!CreateChildProcess(szCmdline))
		return NULL;
	if(!ReadFromPipe(buffer))
		return NULL;
#else
	if(strlen(adbPath) > 0)
		sprintf(cmd, "%s devices", adbPath);
	else
		return NULL;
	printf( "command start: %s \n",cmd);
	fp = popen (cmd, "r");
	printf( "command end\n");
	if (fp == NULL)
	{
		printf( "The file was not opened\n");
		return NULL;
	}
	while( (numread = fread(buffer, sizeof(char), BUFFER_SIZE, fp)))
	{
		if(numread==0)
			break;
	}
	printf( "The file was %s\n", buffer);
	pclose(fp);
#endif
	buffer[strlen(buffer)]='\0';
	pb = buffer;
	if(strncmp(pb, sigstr,strlen(sigstr)))
		return NULL;
	pb = pb + strlen(sigstr);
	if(!strstr(pb, devstr))
		return NULL;
	printf( "The result is %s\n", pb);
	for(int i=0;i<USB_NUM;i++){
		
	}
	return deviceslist;
}

#ifndef XP_LINUX
__declspec(dllexport) 
#endif
int setupDevice(char* device)
{
    char cmd[CMD_SIZE] = {0};
	int ret = 0;
	
	if(strlen(adbPath) > 0)
		sprintf(cmd, "%s -s %s forward tcp:%d tcp:%d", adbPath, device,LOCAL_PORT,REMOTE_PORT);
	else
		return 0;
		
#ifndef XP_LINUX
	TCHAR szCmdline[CMD_SIZE]={0};
	SECURITY_ATTRIBUTES saAttr; 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 
		
    MultiByteToWideChar(CP_ACP,0,cmd,strlen(cmd),szCmdline,CMD_SIZE); 
   
	if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
		return 0;

	if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
		return 0;

	CreateChildProcess(szCmdline);
	ret = ReadFromPipe(NULL); 
#else
	ret = system(cmd);
#endif
	if(ret)
		return 0;
    return 1;
}

#ifndef XP_LINUX
__declspec(dllexport) 
#endif
int pullfile(char *sfilepath,char *dfilepath)
{
	FILE *fp = 0;
	int numread = 0;
	char buffer[BUFFER_SIZE] = {0};
	char *pb = 0;
	char cmd[CMD_SIZE] = {0};
	int ret = 0;
	
#ifndef XP_LINUX
	TCHAR szCmdline[CMD_SIZE]={0};
	SECURITY_ATTRIBUTES saAttr; 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 
	
	if(strlen(adbPath) > 0)
		sprintf(cmd, "%s pull %s %s", adbPath,sfilepath,dfilepath);
	else
		return 0;
	
	MultiByteToWideChar(CP_ACP,0,cmd,strlen(cmd),szCmdline,CMD_SIZE); 
	
	if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
		return 0;

	if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
		return 0;
	if(!CreateChildProcess(szCmdline))
		return 0;
	if(!ReadFromPipe(buffer))
		return 0;
#else
	if(strlen(adbPath) > 0)
		sprintf(cmd, "%s pull %s %s", adbPath,sfilepath,dfilepath);
	else
		return 0;
	printf( "command start: %s \n",cmd);
	fp = popen (cmd, "r");
	printf( "command end\n");
	if (fp == NULL)
	{
		printf( "The file was not opened\n");
		return 0;
	}
	while( (numread = fread(buffer, sizeof(char), BUFFER_SIZE, fp)))
	{
		if(numread==0)
			break;
	}
	printf( "The file was %s\n", buffer);
	pclose(fp);
#endif
	buffer[strlen(buffer)]='\0';
	pb = buffer;
	if(strlen(buffer) > 0)
		return 1;
	return 0;
}

#ifndef XP_LINUX
__declspec(dllexport) 
#endif
int pushfile(char *sfilepath,char *dfilepath)
{
	FILE *fp = 0;
	int numread = 0;
	char buffer[BUFFER_SIZE] = {0};
	char *pb = 0;
	char cmd[CMD_SIZE] = {0};
	int ret = 0;
	
#ifndef XP_LINUX
	TCHAR szCmdline[CMD_SIZE]={0};
	SECURITY_ATTRIBUTES saAttr; 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 
	
	if(strlen(adbPath) > 0)
		sprintf(cmd, "%s push %s %s", adbPath,sfilepath,dfilepath);
	else
		return 0;
	
	MultiByteToWideChar(CP_ACP,0,cmd,strlen(cmd),szCmdline,CMD_SIZE); 
	
	if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
		return 0;

	if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
		return 0;
	if(!CreateChildProcess(szCmdline))
		return 0;
	if(!ReadFromPipe(buffer))
		return 0;
#else
	if(strlen(adbPath) > 0)
		sprintf(cmd, "%s push %s %s", adbPath,sfilepath,dfilepath);
	else
		return 0;
	printf( "command start: %s \n",cmd);
	fp = popen (cmd, "r");
	printf( "command end\n");
	if (fp == NULL)
	{
		printf( "The file was not opened\n");
		return 0;
	}
	while( (numread = fread(buffer, sizeof(char), BUFFER_SIZE, fp)))
	{
		if(numread==0)
			break;
	}
	printf( "The file was %s\n", buffer);
	pclose(fp);
#endif
	buffer[strlen(buffer)]='\0';
	pb = buffer;
	if(strlen(buffer) > 0)
		return 1;
	return 0;
}


#ifndef XP_LINUX
__declspec(dllexport) 
#endif
void setupPath(char *path)
{
	printf( "The path is %s\n", path);
	if(path != NULL)
		strcpy(adbPath,path);
}

