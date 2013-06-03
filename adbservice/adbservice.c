#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef XP_LINUX
#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>


HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
 
BOOL CreateChildProcess(TCHAR *szCmdline); 
BOOL ReadFromPipe(CHAR *pszOutput,int outputSize); 
#endif

#define BUFFER_SIZE 10240
#define CMD_SIZE 1024
#define LOCAL_PORT 10010
#define REMOTE_PORT 10010
char adbPath[BUFFER_SIZE] = {0};

#define RET_SUCCESS 0
#define RET_ERROR 1

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
 
BOOL ReadFromPipe(CHAR *pszOutput, int outputSize) 
{ 
   DWORD dwRead; 
   CHAR chBuf[BUFFER_SIZE] = {0}; 
   BOOL bSuccess = FALSE;
   HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
   CloseHandle(g_hChildStd_OUT_Wr);
   bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFFER_SIZE, &dwRead, NULL);
   if( ! bSuccess || dwRead == 0 ) 
	return RET_ERROR;
	if(pszOutput != NULL)
		strcpy_s(pszOutput,outputSize, chBuf);
    return RET_SUCCESS;
} 
#endif

#ifndef XP_LINUX
__declspec(dllexport) 
#endif
int runCmd(char *cmd, char *retBuf, int retBufSize)
{
	char buffer[BUFFER_SIZE] = {0};
#ifndef XP_LINUX
	TCHAR szCmdline[CMD_SIZE]={0};
	SECURITY_ATTRIBUTES saAttr; 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 
	
	MultiByteToWideChar(CP_ACP,0,cmd,strlen(cmd),szCmdline,CMD_SIZE); 
	
	if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
		return RET_ERROR;

	if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
		return RET_ERROR;
	if(!CreateChildProcess(szCmdline))
		return RET_ERROR;
	if(buffer){
		return ReadFromPipe(buffer,BUFFER_SIZE);
	}else{
		return ReadFromPipe(NULL, 0); 
	}
#else
	FILE *fp = 0;
	int numread = 0;
	char *pb = 0;
	if(buffer){
		fp = popen (cmd, "r");
		if (fp == NULL)
		{
			return RET_ERROR;
		}
		while( (numread = fread(buffer, sizeof(char), BUFFER_SIZE, fp)))
		{
			if(numread==0)
				break;
		}
		pclose(fp);
		buffer[strlen(buffer)]='\0';
		pb = buffer;
		if(strlen(buffer) > 0){
			strcpy_s(retBuf,retBufSize, pb);
			return RET_SUCCESS;
		}
		return RET_ERROR;
	}else
		return system(cmd);
#endif
}

#ifndef XP_LINUX
__declspec(dllexport) 
#endif
int findDevice(char *retBuf,int retBufSize)
{
	char *sigstr = "List of devices attached";
	char *devstr = "device";
	char buffer[BUFFER_SIZE] = {0};
	char *pb = 0;
	char cmd[CMD_SIZE] = {0};
	int ret = 0;
	if(strlen(adbPath) > 0){
		sprintf_s(cmd,CMD_SIZE, "%s devices", adbPath);
		ret = runCmd(cmd, buffer,BUFFER_SIZE);
		if(ret){
			pb = buffer;
			if(strncmp(pb, sigstr,strlen(sigstr)))
				return RET_ERROR;
			pb = pb + strlen(sigstr);
			if(strlen(pb)){
				strcpy_s(retBuf,retBufSize, pb);
				return RET_SUCCESS;
			}
		}
	}
	return RET_ERROR;
}

#ifndef XP_LINUX
__declspec(dllexport) 
#endif
int setupDevice(char* device)
{
    char cmd[CMD_SIZE] = {0};
	int ret = 0;
	if(strlen(adbPath) > 0){
		if(device)
			sprintf_s(cmd,CMD_SIZE, "%s -s %s forward tcp:%d tcp:%d", adbPath, device, LOCAL_PORT,REMOTE_PORT);
		else
			sprintf_s(cmd,CMD_SIZE, "%s forward tcp:%d tcp:%d", adbPath, LOCAL_PORT,REMOTE_PORT);
		return runCmd(cmd, NULL,0);
	}
	return RET_ERROR;
}

#ifndef XP_LINUX
__declspec(dllexport) 
#endif
int pullfile(char *device, char *sfilepath,char *dfilepath, char *retBuf,int retBufSize)
{
	char buffer[BUFFER_SIZE] = {0};
	char *pb = 0;
	char cmd[CMD_SIZE] = {0};
	int ret = 0;
	
	if(strlen(adbPath) > 0){
		if(device)
			sprintf_s(cmd,CMD_SIZE, "%s -s %s pull %s %s", adbPath,device ,sfilepath,dfilepath);
		else
			sprintf_s(cmd,CMD_SIZE, "%s pull %s %s", adbPath,sfilepath,dfilepath);
		ret = runCmd(cmd, buffer,BUFFER_SIZE);
		if(ret){
			buffer[strlen(buffer)]='\0';
			pb = buffer;
			if(strlen(buffer) > 0){
				strcpy_s(retBuf,retBufSize, pb);
				if(strstr(pb, "KB/s")>0)
					return RET_SUCCESS;
			}
		}	
	}
	return RET_ERROR;
}

#ifndef XP_LINUX
__declspec(dllexport) 
#endif
int pushfile(char *device, char *sfilepath,char *dfilepath,char *retBuf,int retBufSize)
{
	char buffer[BUFFER_SIZE] = {0};
	char *pb = 0;
	char cmd[CMD_SIZE] = {0};
	int ret = 0;
	
	if(strlen(adbPath) > 0){
		if(device)
			sprintf_s(cmd,CMD_SIZE, "%s -s %s push %s %s", adbPath,device, sfilepath,dfilepath);
		else
			sprintf_s(cmd,CMD_SIZE, "%s push %s %s", adbPath, sfilepath,dfilepath);
		ret = runCmd(cmd, buffer, BUFFER_SIZE);
		if(ret){
			buffer[strlen(buffer)]='\0';
			pb = buffer;
			if(strlen(buffer) > 0){
				strcpy_s(retBuf,retBufSize, pb);
				if(strstr(pb, "KB/s")>0)
					return RET_SUCCESS;
			}
		}
	}
	return RET_ERROR;
}

#ifndef XP_LINUX
__declspec(dllexport) 
#endif
void setupPath(char *path)
{
	if(path != NULL)
		strcpy_s(adbPath,BUFFER_SIZE,path);
}

#ifndef XP_LINUX
__declspec(dllexport)
#endif
int listadbservice(char *retBuf,int retBufSize)
{
#ifndef XP_LINUX
	char buffer[BUFFER_SIZE] = {0};
	char *pb = 0;
	char cmd[CMD_SIZE] = {0};
	int pidlen=0, pid=0,ret=0;
	sprintf_s(cmd,CMD_SIZE, "cmd.exe /c netstat -ano | findstr 5037");
	ret = runCmd(cmd, buffer,BUFFER_SIZE);
	pb = strstr(buffer, "LISTENING");
	if(pb){
		pb += strlen("LISTENING");
		pidlen = strstr(pb, "\r\n") - pb;
		pb[pidlen] = 0;
		pid = atoi(pb);
		if(pid > 0){
			sprintf_s(cmd,CMD_SIZE, "cmd.exe /c Tasklist | findstr %d", pid);
			ret = runCmd(cmd, buffer,BUFFER_SIZE);
			if(ret)
				strcpy_s(retBuf,retBufSize, buffer);
			return ret;
//			sprintf_s(cmd,CMD_SIZE, "cmd.exe /c taskkill /f /pid %d", pid);
		}
	}
#endif
	return RET_ERROR;
}
