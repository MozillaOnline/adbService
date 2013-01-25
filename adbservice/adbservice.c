#include <stdlib.h>
#include <stdio.h>

#ifndef XP_LINUX
#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>

#define BUFSIZE 4096 

HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
 
BOOL CreateChildProcess(TCHAR *szCmdline); 
BOOL ReadFromPipe(CHAR *pszOutput); 
#endif

#define BUFFER_SIZE 1024 
#define CMD_SIZE 128
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
   
   if (bSuccess )  
   {
      CloseHandle(piProcInfo.hProcess);
      CloseHandle(piProcInfo.hThread);
   }
   return bSuccess;
}
 
BOOL ReadFromPipe(CHAR *pszOutput) 
{ 
   DWORD dwRead, dwWritten; 
   CHAR chBuf[BUFSIZE] = {0}; 
   BOOL bSuccess = FALSE;
   HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
   CloseHandle(g_hChildStd_OUT_Wr);
   bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
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
	
#ifndef XP_LINUX
	TCHAR szCmdline[CMD_SIZE]={0};
	SECURITY_ATTRIBUTES saAttr; 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 
	
	if(strlen(adbPath) > 0)
		sprintf(cmd, "%s devices", adbPath);
	else
		return 0;
	
	MultiByteToWideChar(CP_ACP,0,cmd,strlen(cmd),szCmdline,CMD_SIZE); 
	
	if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
		return 0;

	if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
		return 0;

	CreateChildProcess(szCmdline);
	ReadFromPipe(buffer); 
#else
	if(strlen(adbPath) > 0)
		sprintf(cmd, "%s devices", adbPath);
	else
		return 0;
	strcat(cmd, " > "); 
	strcat(cmd, logname); 
	ret = system(cmd);
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
#endif
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

#ifndef XP_LINUX
__declspec(dllexport) 
#endif
int setupDevice()
{
    char cmd[CMD_SIZE] = {0};
	int ret = 0;
		
#ifndef XP_LINUX
	TCHAR szCmdline[CMD_SIZE]={0};
	SECURITY_ATTRIBUTES saAttr; 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 
	
	if(strlen(adbPath) > 0)
		sprintf(cmd, "%s forward tcp:%d tcp:%d", adbPath, LOCAL_PORT,REMOTE_PORT);
	else
		return 0;
		
    MultiByteToWideChar(CP_ACP,0,cmd,strlen(cmd),szCmdline,CMD_SIZE); 
   
	if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
		return 0;

	if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
		return 0;

	CreateChildProcess(szCmdline);
	ret = ReadFromPipe(NULL); 
#else

	if(strlen(adbPath) > 0)
		sprintf(cmd, "%s forward tcp:%d tcp:%d", adbPath, LOCAL_PORT,REMOTE_PORT);
	else
		return 0;
	ret = system(cmd);
#endif

	if(ret)
		return 0;*/
    return 1;
}
#ifndef XP_LINUX
__declspec(dllexport) 
#endif
void setupPath(char *path)
{
	if(path != NULL)
		strcpy(adbPath,path);
}

