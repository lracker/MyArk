#pragma once
#include <ntifs.h>
#include <ntddk.h>

//////////////////////////////////////////////////////////////////////////
//////////////////////////// 内存操作 /////////////////////////////////////

//************************************
// Method:    alloc 申请内存
// Returns:   void* 返回内存空间首地址, 申请失败返回NULL
// Parameter: ULONG size 要申请的字节数
//************************************
void* alloc(ULONG size);
//************************************
// Method:    reAlloc 重新分配空间
// Returns:   void*  返回新空间的内存地址
// Parameter: void * src 原始内存空间(必须由alloc函数所返回)
// Parameter: ULONG size 重新分配的字节数
//************************************
void* reAlloc(void* src, ULONG size);

//************************************
// Method:    free 释放内存空间
// Returns:   void
// Parameter: void *
//************************************
void free(void* data);



//////////////////////////////////////////////////////////////////////////
//////////////////////////// 文件操作 /////////////////////////////////////
//************************************
// Method:    createFile 创建文件
// Returns:   NTSTATUS
// Parameter: const wchar_t * filepath 文件路径,路径必须是设备名"\\device\\volumn\\"或符号连接名"\\??\\C:\\1.txt"
// Parameter: ULONG access 访问权限, GENERIC_READ, GENERIC_XXX
// Parameter: ULONG share 文件共享方式: FILE_SHARE_XXX
// Parameter: ULONG openModel  打开方式: FILE_OPEN_IF,FILE_CREATE ...
// Parameter: BOOLEAN isDir 是否为目录
// Parameter: HANDLE * hFile
//************************************
NTSTATUS createFile(wchar_t* filepath,
					ULONG access,
					ULONG share,
					ULONG openModel,
					BOOLEAN isDir,
					HANDLE* hFile);

//************************************
// Method:    getFileSize 获取文件大小
// Returns:   NTSTATUS  
// Parameter: HANDLE hFile 文件句柄
// Parameter: ULONG64 * size  文件大小
//************************************
NTSTATUS getFileSize(HANDLE hFile,
					 ULONG64* size);

//************************************
// Method:    readFile  读取文件内容
// Returns:   NTSTATUS
// Parameter: HANDLE hFile 文件句柄
// Parameter: ULONG offsetLow 文件偏移的低32位, 从此位置开始读取
// Parameter: ULONG offsetHig 文件偏移的高32位, 从此位置开始读取
// Parameter: ULONG sizeToRead 要读取的字节数
// Parameter: PVOID pBuff 保存文件内容的缓冲区 , 需要自己申请内存空间.
// Parameter: ULONG * read 实际读取到的字节数
//************************************
NTSTATUS readFile(HANDLE hFile,
				  ULONG offsetLow,
				  ULONG offsetHig,
				  ULONG sizeToRead,
				  PVOID pBuff,
				  ULONG* read);

NTSTATUS writeFile(HANDLE hFile,
				   ULONG offsetLow,
				   ULONG offsetHig,
				   ULONG sizeToWrite,
				   PVOID pBuff,
				   ULONG* write);

NTSTATUS copyFile(wchar_t* srcPath,
				  wchar_t* destPath);

NTSTATUS moveFile(wchar_t* srcPath,
				  wchar_t* destPath);

NTSTATUS removeFile(wchar_t* path);


//************************************
// Method:    listDirGet 列出一个目录下的文件和文件夹
// Returns:   NTSTATUS 
// Parameter: wchar_t * dir 目录名, 目录名必须是设备名"\\device\\volumn\\"或符号连接名"\\??\\C:\\1.txt"
// Parameter: FILE_BOTH_DIR_INFORMATION ** fileInfo 保存文件内容的缓冲区, 该缓冲区由函数内部申请空间, 必须通过函数`listDirFree`来释放.
// Parameter: ULONG maxFileCount 要获取的最大文件个数.如果目录下有100个文件,此参数传了5,则只能获取到5个文件.
//************************************
NTSTATUS listDirGet(wchar_t* dir ,
					FILE_BOTH_DIR_INFORMATION** fileInfo,
					ULONG maxFileCount);



//************************************
// Method:    firstFile 获取一个目录下的第一个文件
// Returns:   NTSTATUS
// Parameter: wchar_t * dir 目录名, 目录名必须是设备名"\\device\\volumn\\"或符号连接名"\\??\\C:\\1.txt"
// Parameter: HANDLE * hFind 函数输出值,是一个目录句柄
// Parameter: FILE_BOTH_DIR_INFORMATION * fileInfo 保存文件内容的缓冲区, 
//								这个缓冲区的大小最好是: sizeof(FILE_BOTH_DIR_INFORMATION) + 267*2
//************************************
NTSTATUS firstFile(wchar_t* dir, HANDLE* hFind, FILE_BOTH_DIR_INFORMATION* fileInfo,int size);

//************************************
// Method:    nextFile 获取一个目录下的下一个文件. 
// Returns:   NTSTATUS
// Parameter: HANDLE hFind  目录句柄, 该句柄是由firstFile函数所返回 .
// Parameter: FILE_BOTH_DIR_INFORMATION * fileInfo 保存文件信息的缓冲区. 这个缓冲区的大小最好是: sizeof(FILE_BOTH_DIR_INFORMATION) + 267*2
//************************************
NTSTATUS nextFile(HANDLE hFind, FILE_BOTH_DIR_INFORMATION* fileInfo, int size);


void listDirFree(FILE_BOTH_DIR_INFORMATION* fileInfo);

#define ListDirNext(Type,fileinfo) ((Type*)((ULONG_PTR)fileinfo + fileinfo->NextEntryOffset))
#define ListDirForEach(FileInfoType,fileInfo, iterator) \
	for (FileInfoType* iterator = fileInfo; \
		iterator->NextEntryOffset != 0;	 \
		iterator = ListDirNext(FileInfoType,iterator))


#pragma pack(1)
typedef struct _ServiceDesriptorEntry
{
	ULONG *ServiceTableBase;        // 服务表基址
	ULONG *ServiceCounterTableBase; // 计数表基址
	ULONG NumberOfServices;         // 表中项的个数
	UCHAR *ParamTableBase;          // 参数表基址
}SSDTEntry, *PSSDTEntry;
#pragma pack()

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;    //双向链表
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	// ...
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;


void disablePageWriteProtect( );
void enablePageWriteProtect( );
VOID DriverUnload( PDRIVER_OBJECT pDriver );