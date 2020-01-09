#pragma once
#include <ntifs.h>

// 遍历驱动
ULONG EnumDriver(PVOID pOutPut, ULONG ulBuffSize, PDRIVER_OBJECT pDriver);

// 隐藏驱动
UINT8 HideDriver(UNICODE_STRING DriverName, PDRIVER_OBJECT pDriver);

// 遍历进程
ULONG EnumProcess(PVOID pOutPut, ULONG ulBuffSize);

// 遍历线程
ULONG EnumThread(PVOID pOutPut, ULONG ulBuffSize, PVOID pPID);

// 遍历模块
ULONG EnumModule(PVOID pOutPut, ULONG ulBuffSize, PVOID pPID);

// 隐藏进程
VOID HideProcess(PVOID pPID);

// 结束进程
VOID TerminateProcess(PVOID pPID);

// 遍历目录下所有的文件
ULONG EnumFile(PVOID pOutPut, ULONG ulBuffSize, PVOID DirectoryName);

// FindFirstFile
BOOLEAN KernelFindFirstFile(HANDLE hFile, ULONG ulLen, PFILE_BOTH_DIR_INFORMATION pDIr, ULONG ulFirstLen, PFILE_BOTH_DIR_INFORMATION pFirstDir);

// FindNextFile
BOOLEAN KernelFindNextFile(PFILE_BOTH_DIR_INFORMATION pDirList, PFILE_BOTH_DIR_INFORMATION pDirInfo, LONG* Loc);

// 打开文件
HANDLE KernelCreateFile(PUNICODE_STRING pStrFile, BOOLEAN bIsDIr);

// 删除文件
BOOLEAN DeleteFiles(PVOID FileName);

// 获取文件大小
LONGLONG GetFileSize(PUNICODE_STRING pFileStr);

// 遍历IDT表
BOOLEAN EnumIDT(PVOID pOutPut, ULONG ulBuffSize);

// 遍历GDT表
ULONG EnumGDT(PVOID pOutPut, ULONG ulBuffSize);

// 遍历SSDT表
ULONG EnumSSDT(PVOID pOutPut, ULONG ulBuffSize);

// 遍历注册表
ULONG EnumRegister(PVOID pOutPut, ULONG ulBuffSize, PVOID RegisterName);

// 创建子项
ULONG NewReg(PVOID pRegisterName);

// 删除子项
ULONG DeleteReg(PVOID pRegisterName);