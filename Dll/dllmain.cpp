//dllmain.cpp∶定义DLL应用程序的A口点。
#include "pch.h"
#include "framework.h"
#include "stdarg.h"
#include "windows.h"
#include <iostream>
#include "detours.h"
#include<Psapi.h>
#include<WinSock2.h>
#include <Ws2tcpip.h>
#include<winreg.h>

#pragma comment(lib, "detours.lib")
#pragma comment(lib,"ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define INJECT_FILENAME "FCInject_Qt.exe"

class Info
{
public:
	int ParaNum;
	char ParaName[10][50];
	char ParaValue[10][256];
	SYSTEMTIME st;
	//Info()
	//{
	//	ParaNum = 0;
	//	ParaName[10][50] = { 0 };
	//	ParaValue[10][256] = { 0 };
	//}
};

Info info;
HANDLE hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, L"HookSemaphore");
HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(Info), L"ShareMemory");
LPVOID lpBase = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

//对messagebox的截获
//定义和引入需要Hook的函数,和替换的函数
static int(WINAPI* OldMesssageBoxA) (_In_opt_ HWND hWnd, _In_opt_ LPCSTR lpText, _In_opt_ LPCSTR lpCaption, _In_opt_ UINT uType) = MessageBoxA;
static int (WINAPI* OldMesssageBoxW)(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_opt_ UINT uType) = MessageBoxW;

extern "C" __declspec(dllexport)int WINAPI NewMesssageBoxA(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_ UINT uType)
{
	GetLocalTime(&info.st);
	info.ParaNum = 5;
	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[4], "hWnd");
	sprintf_s(info.ParaName[1], "lpTest");
	sprintf_s(info.ParaName[2], "lpCaption");
	sprintf_s(info.ParaName[3], "uType");

	sprintf_s(info.ParaValue[0], "%s", "MessageBoxA");
	sprintf_s(info.ParaValue[4], "%08X", (int)hWnd);
	sprintf_s(info.ParaValue[1], "%ls", lpText);
	sprintf_s(info.ParaValue[2], "%ls", lpCaption);
	sprintf_s(info.ParaValue[3], "%08X", uType);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);

	return OldMesssageBoxA(NULL, "new MesssageBoxA!", "Hooked!", MB_OK);
}

extern "C" __declspec(dllexport)int WINAPI NewMesssageBoxW(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_opt_ UINT uType)
{
	GetLocalTime(&info.st);

	info.ParaNum = 5;
	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[4], "hWnd");
	sprintf_s(info.ParaName[1], "lpTest");
	sprintf_s(info.ParaName[2], "lpCaption");
	sprintf_s(info.ParaName[3], "uType");

	sprintf_s(info.ParaValue[0], "%s", "MessageBoxW");
	sprintf_s(info.ParaValue[4], "%08X", (int)hWnd);
	sprintf_s(info.ParaValue[1], "%ls", lpText);
	sprintf_s(info.ParaValue[2], "%ls", lpCaption);
	sprintf_s(info.ParaValue[3], "%08X", uType);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);

	return OldMesssageBoxW(NULL, L"new MessageBoxW!", L"Hooked!", MB_OK);
}

//文件操作**********create/read/write/delete****************
//创建文件操作
static HANDLE(WINAPI* OldCreateFile)(
	LPCTSTR lpFileName,//文件名
	DWORD dwDesiredAccess,// 访问模式
	DWORD dwShareMode,//共享模式
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, // 安全属性(也即销毁方式)
	DWORD dwCreationDisposition,//how to create
	DWORD dwFlagsAndAttributes,//文件属性
	HANDLE hTemplateFile//模板文件句柄
	) = CreateFile;

extern "C" __declspec(dllexport)HANDLE WINAPI NewCreateFile(
	LPCTSTR lpFileName,//文件名
	DWORD dwDesiredAccess,// 访问模式
	DWORD dwShareMode,//共享模式
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, // 安全属性(也即销毁方式)
	DWORD dwCreationDisposition,//how to create
	DWORD dwFlagsAndAttributes,//文件属性
	HANDLE hTemplateFile//模板文件句柄
)
{
	GetLocalTime(&info.st);
	info.ParaNum = 8;
	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[1], "lpFileName");
	sprintf_s(info.ParaName[2], "dwDesiredAccess");
	sprintf_s(info.ParaName[3], "dwShareMode");
	sprintf_s(info.ParaName[4], "lpSecurityAttributes");
	sprintf_s(info.ParaName[5], "dwCreationDisposition");
	sprintf_s(info.ParaName[6], "dwFlagsAndAttributes");
	sprintf_s(info.ParaName[7], "hTemplateFile");
	sprintf_s(info.ParaValue[0], "%s", "CreateFile");

	char temp[100] = { 0 };
	//将lpFileName中的unicode转换为多字节，并存储在temp中
	WideCharToMultiByte(CP_ACP, 0, lpFileName, wcslen(lpFileName), temp, sizeof(temp), NULL, NULL);
	strcpy_s(info.ParaValue[1], temp);
	sprintf_s(info.ParaValue[2], "%08X", dwDesiredAccess);
	sprintf_s(info.ParaValue[3], "%08X", dwShareMode);
	sprintf_s(info.ParaValue[4], "%08X", (int)&lpSecurityAttributes);
	sprintf_s(info.ParaValue[5], "%08X", dwCreationDisposition);
	sprintf_s(info.ParaValue[6], "%08X", dwFlagsAndAttributes);
	sprintf_s(info.ParaValue[7], "%08X", hTemplateFile);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

//文件读操作
static BOOL(WINAPI* OldReadFile)(
	_In_ HANDLE hFile,
	__out_data_source(FILE) LPVOID lpBuffer,
	_In_ DWORD nNumberOfBytesToRead,
	_Out_opt_ LPDWORD lpNumberOfBytesRead,
	_Inout_opt_ LPOVERLAPPED lpOverlapped
	) = ReadFile;
extern "C" __declspec(dllexport)BOOL WINAPI NewReadFile(
	_In_ HANDLE hFile,
	__out_data_source(FILE) LPVOID lpBuffer,
	_In_ DWORD nNumberOfBytesToRead,
	_Out_opt_ LPDWORD lpNumberOfBytesRead,
	_Inout_opt_ LPOVERLAPPED lpOverlapped
)
{
	GetLocalTime(&info.st);
	info.ParaNum = 6;
	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[1], "hFile");
	sprintf_s(info.ParaName[2], "lpBuffer");
	sprintf_s(info.ParaName[3], "nNumberOfBytesToRead");
	sprintf_s(info.ParaName[4], "lpNumberOfBytesRead");
	sprintf_s(info.ParaName[5], "lpOverlapped");

	sprintf_s(info.ParaValue[0], "%s", "ReadFile");
	sprintf_s(info.ParaValue[1], "%08X", hFile);
	sprintf_s(info.ParaValue[2], "%08X", lpBuffer);
	sprintf_s(info.ParaValue[3], "%08X", nNumberOfBytesToRead);
	sprintf_s(info.ParaValue[4], "%08X", lpNumberOfBytesRead);
	sprintf_s(info.ParaValue[5], "%08X", lpOverlapped);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}
//文件写操作
static BOOL(WINAPI* OldWriteFile)(
	_In_ HANDLE hFile,
	_In_reads_bytes_opt_(nNumberOfBytesToWrite) LPCVOID lpBuffer,
	_In_ DWORD nNumberOfBytesToWrite,
	_Out_opt_ LPDWORD lpNumberOfBytesWritten,
	_Inout_opt_ LPOVERLAPPED lpOverlapped
	) = WriteFile;

extern "C" __declspec(dllexport) BOOL WINAPI NewWriteFile(
	_In_ HANDLE hFile,
	_In_reads_bytes_opt_(nNumberOfBytesToWrite) LPCVOID lpBuffer,
	_In_ DWORD nNumberOfBytesToWrite,
	_Out_opt_ LPDWORD lpNumberOfBytesWritten,
	_Inout_opt_ LPOVERLAPPED lpOverlapped
)
{
	GetLocalTime(&info.st);
	info.ParaNum = 6;
	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[1], "hFile");
	sprintf_s(info.ParaName[2], "lpBuffer");
	sprintf_s(info.ParaName[3], "nNumberOfBytesToWrite");
	sprintf_s(info.ParaName[4], "lpNumberOfBytesWritten");
	sprintf_s(info.ParaName[5], "lpOverlapped");

	sprintf_s(info.ParaValue[0], "%s", "WriteFile");
	sprintf_s(info.ParaValue[1], "%08X", hFile);
	sprintf_s(info.ParaValue[2], "%08X", lpBuffer);
	sprintf_s(info.ParaValue[3], "%08X", nNumberOfBytesToWrite);
	sprintf_s(info.ParaValue[4], "%08X", lpNumberOfBytesWritten);
	sprintf_s(info.ParaValue[5], "%08X", lpOverlapped);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

//文件删除操作
static BOOL(WINAPI* OldDeleteFile)(
	_In_ LPCWSTR lpFileName
	) = DeleteFile;

extern "C" __declspec(dllexport) BOOL WINAPI NewDeleteFile(
	_In_ LPCWSTR lpFileName
)
{
	GetLocalTime(&info.st);
	info.ParaNum = 2;
	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[1], "lpFileName");

	sprintf_s(info.ParaValue[0], "%s", "DeleteFile");
	char temp[70];
	memset(temp, 0, sizeof(temp));
	WideCharToMultiByte(CP_ACP, 0, lpFileName, wcslen(lpFileName), temp, sizeof(temp), NULL, NULL);
	strcpy_s(info.ParaValue[1], temp);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldDeleteFile(lpFileName);
}

//堆操作 Heap***********create/free/destroy*********************

//创建堆操作
static HANDLE(WINAPI* OldHeapCreate)(
	DWORD fIOoptions,
	SIZE_T dwInitialSize,
	SIZE_T dwMaximumSize
	) = HeapCreate;

extern "C" __declspec(dllexport)HANDLE WINAPI NewHeapCreate(DWORD fIOoptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize)
{
	HANDLE hHeap = OldHeapCreate(fIOoptions, dwInitialSize, dwMaximumSize);

	GetLocalTime(&info.st);
	info.ParaNum = 5;

	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[2], "fIOoptions");
	sprintf_s(info.ParaName[3], "dwInitialSize");
	sprintf_s(info.ParaName[4], "dwMaximumSize");
	sprintf_s(info.ParaName[1], "Handle");

	sprintf_s(info.ParaValue[0], "%s", "HeapCreate");
	sprintf_s(info.ParaValue[2], "%08X", fIOoptions);
	sprintf_s(info.ParaValue[3], "%08X", dwInitialSize);
	sprintf_s(info.ParaValue[4], "%08X", dwMaximumSize);
	sprintf_s(info.ParaValue[1], "%08X", hHeap);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return hHeap;
}
//free堆操作
//堆操作Free
static BOOL(WINAPI* OldHeapFree)(
	_Inout_ HANDLE hHeap,
	_In_ DWORD dwFlags,
	_Frees_ptr_opt_ LPVOID lpMem
	) = HeapFree;
extern "C" __declspec(dllexport)BOOL WINAPI NewHeapFree(
	_Inout_ HANDLE hHeap,
	_In_ DWORD dwFlags,
	_Frees_ptr_opt_ LPVOID lpMem
)
{
	GetLocalTime(&info.st);
	info.ParaNum = 4;

	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[1], "hHeap");
	sprintf_s(info.ParaName[2], "dwFlags");
	sprintf_s(info.ParaName[3], "lpMem");

	sprintf_s(info.ParaValue[0], "%s", "HeapFree");
	sprintf_s(info.ParaValue[1], "%08X", hHeap);
	sprintf_s(info.ParaValue[2], "%08X", dwFlags);
	sprintf_s(info.ParaValue[3], "%08X", lpMem);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldHeapFree(hHeap, dwFlags, lpMem);
}

//堆操作Destroy
static BOOL(WINAPI* OldHeapDestroy)(HANDLE) = HeapDestroy;
extern "C" __declspec(dllexport)BOOL WINAPI NewHeapDestroy(HANDLE hHeap)
{
	GetLocalTime(&info.st);
	info.ParaNum = 4;
	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[1], "hHeap");

	sprintf_s(info.ParaValue[0], "%s", "HeapDestroy");
	sprintf_s(info.ParaValue[1], "%08X", hHeap);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldHeapDestroy(hHeap);
}

//注册表操作****create/open/write/close/delete

//注册表建立操作 Create
static LSTATUS(WINAPI* OldRegCreateKey)(
	_In_ HKEY hKey,
	_In_ LPCWSTR lpSubKey,
	_Reserved_ DWORD Reserved,
	_In_opt_ LPWSTR lpClass,
	_In_ DWORD dwOptions,
	_In_ REGSAM samDesired,
	_In_opt_ CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_Out_ PHKEY phkResult,
	_Out_opt_ LPDWORD lpdwDisposition
	) = RegCreateKeyEx;
extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegCreateKey(
	_In_ HKEY hKey,
	_In_ LPCWSTR lpSubKey,
	_Reserved_ DWORD Reserved,
	_In_opt_ LPWSTR lpClass,
	_In_ DWORD dwOptions,
	_In_ REGSAM samDesired,
	_In_opt_ CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_Out_ PHKEY phkResult,
	_Out_opt_ LPDWORD lpdwDisposition
)
{
	GetLocalTime(&info.st);
	info.ParaNum = 10;
	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[1], "hKey");
	sprintf_s(info.ParaName[2], "lpSubKey");
	sprintf_s(info.ParaName[3], "Reserved");
	sprintf_s(info.ParaName[4], "lpClass");
	sprintf_s(info.ParaName[5], "dwOptions");
	sprintf_s(info.ParaName[6], "samDesired");
	sprintf_s(info.ParaName[7], "lpSecurityAttributes");
	sprintf_s(info.ParaName[8], "phkResult");
	sprintf_s(info.ParaName[9], "lpdwDisposition");

	sprintf_s(info.ParaValue[0], "%s", "RegCreateKey");
	sprintf_s(info.ParaValue[1], "%08X", hKey);
	sprintf_s(info.ParaValue[2], "%08X", lpSubKey);
	sprintf_s(info.ParaValue[3], "%08X", Reserved);
	sprintf_s(info.ParaValue[4], "%08X", lpClass);
	sprintf_s(info.ParaValue[5], "%08X", dwOptions);
	sprintf_s(info.ParaValue[6], "%08X", samDesired);
	sprintf_s(info.ParaValue[7], "%08X", lpSecurityAttributes);
	sprintf_s(info.ParaValue[8], "%08X", phkResult);
	sprintf_s(info.ParaValue[9], "%08X", lpdwDisposition);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldRegCreateKey(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

//注册表打开操作Open
static LSTATUS(WINAPI* OldRegOpenKey)(
	_In_ HKEY hKey,
	_In_opt_ LPCWSTR lpSubKey,
	_In_opt_ DWORD ulOptions,
	_In_ REGSAM samDesired,
	_Out_ PHKEY phkResult
	) = RegOpenKeyEx;
extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegOpenKey(
	_In_ HKEY hKey,
	_In_opt_ LPCWSTR lpSubKey,
	_In_opt_ DWORD ulOptions,
	_In_ REGSAM samDesired,
	_Out_ PHKEY phkResult
)
{
	GetLocalTime(&info.st);
	info.ParaNum = 6;
	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[1], "hKey");
	sprintf_s(info.ParaName[2], "lpSubKey");
	sprintf_s(info.ParaName[3], "ulOptions");
	sprintf_s(info.ParaName[4], "samDesired");
	sprintf_s(info.ParaName[5], "phkResult");

	sprintf_s(info.ParaValue[0], "%s", "RegOpenKey");
	sprintf_s(info.ParaValue[1], "%08X", hKey);

	char temp[100] = { 0 };
	WideCharToMultiByte(CP_ACP, 0, lpSubKey, wcslen(lpSubKey), temp, sizeof(temp), NULL, NULL);
	strcpy_s(info.ParaValue[2], temp);

	sprintf_s(info.ParaValue[3], "%08X", ulOptions);
	sprintf_s(info.ParaValue[4], "%08X", samDesired);
	sprintf_s(info.ParaValue[5], "%08X", phkResult);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldRegOpenKey(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

//注册表写操作Write
static LSTATUS(WINAPI* OldRegSetValue)(
	_In_ HKEY hKey,
	_In_opt_ LPCWSTR lpValueName,
	_Reserved_ DWORD Reserved,
	_In_ DWORD dwType,
	_In_reads_bytes_opt_(cbData) CONST BYTE* lpData,
	_In_ DWORD cbData
	) = RegSetValueEx;
extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegSetValue(
	_In_ HKEY hKey,
	_In_opt_ LPCWSTR lpValueName,
	_Reserved_ DWORD Reserved,
	_In_ DWORD dwType,
	_In_reads_bytes_opt_(cbData) CONST BYTE * lpData,
	_In_ DWORD cbData
)
{
	GetLocalTime(&info.st);
	info.ParaNum = 7;
	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[1], "hKey");
	sprintf_s(info.ParaName[2], "lpValueName");
	sprintf_s(info.ParaName[3], "Reserved");
	sprintf_s(info.ParaName[4], "dwType");
	sprintf_s(info.ParaName[5], "lpData");
	sprintf_s(info.ParaName[6], "cbData");

	sprintf_s(info.ParaValue[0], "%s", "RegSetValue");
	sprintf_s(info.ParaValue[1], "%08X", hKey);

	char temp[70] = { 0 };
	WideCharToMultiByte(CP_ACP, 0, lpValueName, wcslen(lpValueName), temp, sizeof(temp), NULL, NULL);
	strcpy_s(info.ParaValue[2], temp);

	sprintf_s(info.ParaValue[3], "%08X", Reserved);
	sprintf_s(info.ParaValue[4], "%08X", dwType);

	memset(temp, 0, sizeof(temp));
	WideCharToMultiByte(CP_ACP, 0, (LPCWCH)lpData, wcslen((LPCWCH)lpData), temp, sizeof(temp), NULL, NULL);
	strcpy_s(info.ParaValue[5], temp);

	sprintf_s(info.ParaValue[6], "%08X", cbData);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldRegSetValue(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

//注册表关闭Close
static LSTATUS(WINAPI* OldRegCloseKey)(
	_In_ HKEY hKey
	) = RegCloseKey;
extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegCloseKey(
	_In_ HKEY hKey
)
{
	GetLocalTime(&info.st);
	info.ParaNum = 2;
	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[1], "hKey");

	sprintf_s(info.ParaValue[0], "%s", "RegClose");
	sprintf_s(info.ParaValue[1], "%08X", hKey);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldRegCloseKey(hKey);
}

//注册表删除Delete
static LSTATUS(WINAPI* OldRegDeleteValue)(
	_In_ HKEY hKey,
	_In_opt_ LPCWSTR lpValueName
	) = RegDeleteValue;
extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegDeleteValue(
	_In_ HKEY hKey,
	_In_opt_ LPCWSTR lpValueName
)
{
	GetLocalTime(&info.st);
	info.ParaNum = 3;

	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[1], "hKey");
	sprintf_s(info.ParaName[2], "lpValueName");

	sprintf_s(info.ParaValue[0], "%s", "RegDeleteValue");
	sprintf_s(info.ParaValue[1], "%08X", hKey);

	char temp[100] = { 0 };
	WideCharToMultiByte(CP_ACP, 0, lpValueName, wcslen(lpValueName), temp, sizeof(temp), NULL, NULL);
	strcpy_s(info.ParaValue[2], temp);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldRegCloseKey(hKey);
}

//网络通信
static SOCKET(WINAPI* OldSocket)(
	_In_ int af,
	_In_ int type,
	_In_ int protocol
	) = socket;
extern "C" __declspec(dllexport) SOCKET WINAPI NewSocket(
	_In_ int af,
	_In_ int type,
	_In_ int protocol
)
{
	GetLocalTime(&info.st);
	info.ParaNum = 4;
	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[1], "af");
	sprintf_s(info.ParaName[2], "type");
	sprintf_s(info.ParaName[3], "protocol");

	sprintf_s(info.ParaValue[0], "%s", "Socket");
	sprintf_s(info.ParaValue[1], "%08X", af);
	sprintf_s(info.ParaValue[2], "%08X", type);
	sprintf_s(info.ParaValue[3], "%08X", protocol);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldSocket(af, type, protocol);
}

static int (WINAPI* OldBind)(
	_In_ SOCKET s,
	_In_reads_bytes_(namelen) const struct sockaddr FAR* name,
	_In_ int namelen
	) = bind;
extern "C" __declspec(dllexport) int WINAPI NewBind(
	_In_ SOCKET s,
	_In_reads_bytes_(namelen) const struct sockaddr FAR * name,
	_In_ int namelen
)
{
	GetLocalTime(&info.st);
	info.ParaNum = 6;
	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[1], "s");
	sprintf_s(info.ParaName[2], "name");
	sprintf_s(info.ParaName[3], "namelen");
	sprintf_s(info.ParaName[4], "IP");
	sprintf_s(info.ParaName[5], "port");

	sprintf_s(info.ParaValue[0], "%s", "Bind");
	sprintf_s(info.ParaValue[1], "%08X", s);
	sprintf_s(info.ParaValue[2], "%08X", name);
	sprintf_s(info.ParaValue[3], "%08X", namelen);

	struct sockaddr_in* sock = (struct sockaddr_in*)name;
	int port = ntohs(sock->sin_port);
	sprintf_s(info.ParaValue[4], "%s", inet_ntoa(sock->sin_addr));
	sprintf_s(info.ParaValue[5], "%08X", namelen);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldBind(s, name, namelen);
}

static int (WINAPI* OldConnect)(
	_In_ SOCKET s,
	_In_reads_bytes_(namelen) const struct sockaddr FAR* name,
	_In_ int namelen
	) = connect;
extern "C" __declspec(dllexport) int WINAPI NewConnect(
	_In_ SOCKET s,
	_In_reads_bytes_(namelen) const struct sockaddr FAR * name,
	_In_ int namelen
)
{
	GetLocalTime(&info.st);
	info.ParaNum = 6;
	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[1], "s");
	sprintf_s(info.ParaName[2], "name");
	sprintf_s(info.ParaName[3], "namelen");
	sprintf_s(info.ParaName[4], "IP");
	sprintf_s(info.ParaName[5], "port");

	sprintf_s(info.ParaValue[0], "%s", "Connect");
	sprintf_s(info.ParaValue[1], "%08X", s);
	sprintf_s(info.ParaValue[2], "%08X", name);
	sprintf_s(info.ParaValue[3], "%08X", namelen);

	struct sockaddr_in* sock = (struct sockaddr_in*)name;
	int port = ntohs(sock->sin_port);
	sprintf_s(info.ParaValue[4], "%s", inet_ntoa(sock->sin_addr));
	sprintf_s(info.ParaValue[5], "%08X", namelen);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldConnect(s, name, namelen);
}

static int  (WINAPI* OldSend)(
	_In_ SOCKET s,
	_In_reads_bytes_(len) const char FAR* buf,
	_In_ int len,
	_In_ int flags
	) = send;
extern "C" __declspec(dllexport) int WINAPI NewSend(
	_In_ SOCKET s,
	_In_reads_bytes_(len) const char FAR * buf,
	_In_ int len,
	_In_ int flags
)
{
	GetLocalTime(&info.st);
	info.ParaNum = 6;
	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[1], "s");
	sprintf_s(info.ParaName[2], "buf");
	sprintf_s(info.ParaName[3], "len");
	sprintf_s(info.ParaName[4], "flags");
	sprintf_s(info.ParaName[5], "data");

	sprintf_s(info.ParaValue[0], "%s", "Send");
	sprintf_s(info.ParaValue[1], "%08X", s);
	sprintf_s(info.ParaValue[2], "%08X", buf);
	sprintf_s(info.ParaValue[3], "%08X", len);
	sprintf_s(info.ParaValue[4], "%08X", flags);
	sprintf_s(info.ParaValue[5], "%s", buf);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldSend(s, buf, len, flags);
}

static int (WINAPI* OldRecv)(
	_In_ SOCKET s,
	_Out_writes_bytes_to_(len, return) __out_data_source(NETWORK) char FAR* buf,
	_In_ int len,
	_In_ int flags
	) = recv;
extern "C" __declspec(dllexport) int WINAPI NewRecv(
	_In_ SOCKET s,
	_Out_writes_bytes_to_(len, return) __out_data_source(NETWORK) char FAR* buf,
	_In_ int len,
	_In_ int flags
)
{
	GetLocalTime(&info.st);
	info.ParaNum = 5;
	sprintf_s(info.ParaName[0], "API Name");
	sprintf_s(info.ParaName[1], "s");
	sprintf_s(info.ParaName[2], "buf");
	sprintf_s(info.ParaName[3], "len");
	sprintf_s(info.ParaName[4], "flags");

	sprintf_s(info.ParaValue[0], "%s", "Recv");
	sprintf_s(info.ParaValue[1], "%08X", s);
	sprintf_s(info.ParaValue[2], "%08X", buf);
	sprintf_s(info.ParaValue[3], "%08X", len);
	sprintf_s(info.ParaValue[4], "%08X", flags);

	memcpy(lpBase, &info, sizeof(Info));
	info.ParaNum = 0;
	ReleaseSemaphore(hSemaphore, 1, NULL);
	return OldRecv(s, buf, len, flags);
}

BOOL WINAPI DllMain(HMODULE hModule,
	DWORD ul_reason_for_call,
	LPVOID lpReserved
)
{
	//wchar_t Buf[256];
	//GetModuleFileName(0, Buf, 256);
	//int num = WideCharToMultiByte(CP_OEMCP, NULL, Buf, -1, NULL, 0, NULL, FALSE);
	//char* pchar = new char[num];
	//WideCharToMultiByte(CP_OEMCP, NULL, Buf, -1, pchar, num, NULL, FALSE);
	//if (strcmp(pchar, INJECT_FILENAME) == 0)
	//{
	//	//为该线程
	//	delete pchar;
	//	return TRUE;
	//}

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)OldMesssageBoxW, NewMesssageBoxW);
		DetourAttach(&(PVOID&)OldMesssageBoxA, NewMesssageBoxA);

		DetourAttach(&(PVOID&)OldCreateFile, NewCreateFile);
		DetourAttach(&(PVOID&)OldWriteFile, NewWriteFile);
		DetourAttach(&(PVOID&)OldReadFile, NewReadFile);
		DetourAttach(&(PVOID&)OldDeleteFile, NewDeleteFile);

		DetourAttach(&(PVOID&)OldHeapCreate, NewHeapCreate);
		DetourAttach(&(PVOID&)OldHeapFree, NewHeapFree);
		DetourAttach(&(PVOID&)OldHeapDestroy, NewHeapDestroy);

		DetourAttach(&(PVOID&)OldRegCreateKey, NewRegCreateKey);
		DetourAttach(&(PVOID&)OldRegSetValue, NewRegSetValue);
		DetourAttach(&(PVOID&)OldRegDeleteValue, NewRegDeleteValue);
		DetourAttach(&(PVOID&)OldRegCloseKey, NewRegCloseKey);
		DetourAttach(&(PVOID&)OldRegOpenKey, NewRegOpenKey);

		DetourAttach(&(PVOID&)OldSocket, NewSocket);
		DetourAttach(&(PVOID&)OldBind, NewBind);
		DetourAttach(&(PVOID&)OldSend, NewSend);
		DetourAttach(&(PVOID&)OldConnect, NewConnect);
		DetourAttach(&(PVOID&)OldRecv, NewRecv);

		DetourTransactionCommit();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)OldMesssageBoxW, NewMesssageBoxW);
		DetourDetach(&(PVOID&)OldMesssageBoxA, NewMesssageBoxA);

		DetourDetach(&(PVOID&)OldCreateFile, NewCreateFile);
		DetourDetach(&(PVOID&)OldWriteFile, NewWriteFile);
		DetourDetach(&(PVOID&)OldReadFile, NewReadFile);
		DetourDetach(&(PVOID&)OldDeleteFile, NewDeleteFile);

		DetourDetach(&(PVOID&)OldHeapCreate, NewHeapCreate);
		DetourDetach(&(PVOID&)OldHeapFree, NewHeapFree);
		DetourDetach(&(PVOID&)OldHeapDestroy, NewHeapDestroy);

		DetourDetach(&(PVOID&)OldRegCreateKey, NewRegCreateKey);
		DetourDetach(&(PVOID&)OldRegSetValue, NewRegSetValue);
		DetourDetach(&(PVOID&)OldRegDeleteValue, NewRegDeleteValue);
		DetourDetach(&(PVOID&)OldRegCloseKey, NewRegCloseKey);
		DetourDetach(&(PVOID&)OldRegOpenKey, NewRegOpenKey);

		DetourDetach(&(PVOID&)OldSocket, NewSocket);
		DetourDetach(&(PVOID&)OldBind, NewBind);
		DetourDetach(&(PVOID&)OldSend, NewSend);
		DetourDetach(&(PVOID&)OldConnect, NewConnect);
		DetourDetach(&(PVOID&)OldRecv, NewRecv);

		DetourTransactionCommit();
		break;
	}
	return TRUE;
}