#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef XP_LINUX
#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>
#endif

#define BUFFER_SIZE 10240
#define CMD_SIZE 1024
#define RET_SUCCESS 0
#define RET_ERROR 1
char chBuf[BUFFER_SIZE] ={0}; 

#ifndef XP_LINUX
__declspec(dllexport) 
#endif
char *runCmd(char *cmd)
{
#ifndef XP_LINUX
	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;
	TCHAR szCmdline[CMD_SIZE]={0};
	DWORD dwRead; 
	BOOL bSuccess = FALSE;
	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo;
	HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SECURITY_ATTRIBUTES saAttr; 

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 
	
	memset(chBuf, 0, BUFFER_SIZE);
	MultiByteToWideChar(CP_ACP,0,cmd,strlen(cmd),szCmdline,CMD_SIZE); 
	
	if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) {
		strcpy_s(chBuf,BUFFER_SIZE,"error: CreatePipe.");
		return chBuf;
	}

	if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) ){
		strcpy_s(chBuf,BUFFER_SIZE,"error: SetHandleInformation.");
		return chBuf;
	}
 
	ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO); 
	siStartInfo.hStdError = g_hChildStd_OUT_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.hStdInput = NULL;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
    
	siStartInfo.lpReserved = NULL;
	siStartInfo.lpDesktop = NULL;
	siStartInfo.lpTitle = NULL;
	siStartInfo.dwFlags = STARTF_USESHOWWINDOW;
	siStartInfo.wShowWindow = SW_HIDE;
	siStartInfo.cbReserved2 = 0;
	siStartInfo.lpReserved2 = NULL;

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
	if (bSuccess)  {
		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
		CloseHandle(g_hChildStd_OUT_Wr);
		
		ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFFER_SIZE, &dwRead, NULL);
		return chBuf;
	}else{
		strcpy_s(chBuf,BUFFER_SIZE,"error: CreateProcess.");
		return chBuf;
	}
#else
	FILE *fp = 0;
	int numread = 0;
	char cmdTemp[CMD_SIZE] ={0}; 
	memset(chBuf, 0, BUFFER_SIZE);
	sprintf(cmdTemp, "%s  2>&1", cmd);
	fp = popen (cmdTemp, "r");
	if (fp == NULL){
		strcpy(chBuf,"error: popen.");
		return chBuf;
	}
	while( (numread = fread(chBuf, sizeof(char), BUFFER_SIZE, fp))){
		if(numread==0)
			break;
	}
	pclose(fp);
	return chBuf;
#endif
}
