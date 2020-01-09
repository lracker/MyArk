#include "KernelFunction.h"
#include <ntifs.h>


void* alloc(ULONG size)
{
	return ExAllocatePool(PagedPool, size);
}

void* reAlloc(void* src, ULONG size)
{
	if (!src)
	{
		return NULL;
	}

	void* data = alloc(size);
	RtlCopyMemory(data, src, size);
	ExFreePool(src);
	return data;
}

void free(void* data)
{
	if (data)
	{
		ExFreePool(data);
	}
}

NTSTATUS createFile(wchar_t * filepath,
					ULONG access,
					ULONG share,
					ULONG openModel,
					BOOLEAN isDir,
					HANDLE * hFile)
{

	NTSTATUS status = STATUS_SUCCESS;

	IO_STATUS_BLOCK StatusBlock = { 0 };
	ULONG           ulShareAccess = share;
	ULONG ulCreateOpt = FILE_SYNCHRONOUS_IO_NONALERT;

	UNICODE_STRING path;
	RtlInitUnicodeString(&path, filepath);

	// 1. 初始化OBJECT_ATTRIBUTES的内容
	OBJECT_ATTRIBUTES objAttrib = { 0 };
	ULONG ulAttributes = OBJ_CASE_INSENSITIVE/*不区分大小写*/ | OBJ_KERNEL_HANDLE/*内核句柄*/;
	InitializeObjectAttributes(&objAttrib,    // 返回初始化完毕的结构体
							   &path,      // 文件对象名称
							   ulAttributes,  // 对象属性
							   NULL, NULL);   // 一般为NULL

	// 2. 创建文件对象
	ulCreateOpt |= isDir ? FILE_DIRECTORY_FILE : FILE_NON_DIRECTORY_FILE;

	status = ZwCreateFile(hFile,                 // 返回文件句柄
						  access,				 // 文件操作描述
						  &objAttrib,            // OBJECT_ATTRIBUTES
						  &StatusBlock,          // 接受函数的操作结果
						  0,                     // 初始文件大小
						  FILE_ATTRIBUTE_NORMAL, // 新建文件的属性
						  ulShareAccess,         // 文件共享方式
						  openModel,			 // 文件存在则打开不存在则创建
						  ulCreateOpt,           // 打开操作的附加标志位
						  NULL,                  // 扩展属性区
						  0);                    // 扩展属性区长度
	return status;
}


NTSTATUS getFileSize(HANDLE hFile, ULONG64* size)
{
	IO_STATUS_BLOCK isb = { 0 };
	FILE_STANDARD_INFORMATION fsi = { 0 };
	NTSTATUS status;
	status = ZwQueryInformationFile(hFile, /*文件句柄*/
									&isb, /*完成状态*/
									&fsi, /*保存文件信息的缓冲区*/
									sizeof(fsi), /*缓冲区的字节数*/
									FileStandardInformation/*要获取的信息类型*/);
	if (STATUS_SUCCESS == status)
	{
		// 保存文件字节数
		*size = fsi.EndOfFile.QuadPart;
	}
	return status;
}

NTSTATUS readFile(HANDLE hFile,
				  ULONG offsetLow,
				  ULONG offsetHig,
				  ULONG sizeToRead,
				  PVOID pBuff,
				  ULONG* read)
{
	NTSTATUS status;
	IO_STATUS_BLOCK isb = { 0 };
	LARGE_INTEGER offset;
	// 设置要读取的文件偏移
	offset.HighPart = offsetHig;
	offset.LowPart = offsetLow;

	status = ZwReadFile(hFile,/*文件句柄*/
						NULL,/*事件对象,用于异步IO*/
						NULL,/*APC的完成通知例程:用于异步IO*/
						NULL,/*完成通知例程序的附加参数*/
						&isb,/*IO状态*/
						pBuff,/*保存文件数据的缓冲区*/
						sizeToRead,/*要读取的字节数*/
						&offset,/*要读取的文件位置*/
						NULL);
	if (status == STATUS_SUCCESS)
		*read = isb.Information;
	return status;
}

NTSTATUS writeFile(HANDLE hFile,
				   ULONG offsetLow,
				   ULONG offsetHig,
				   ULONG sizeToWrite,
				   PVOID pBuff,
				   ULONG* write)
{

	NTSTATUS status;
	IO_STATUS_BLOCK isb = { 0 };
	LARGE_INTEGER offset;
	// 设置要写入的文件偏移
	offset.HighPart = offsetHig;
	offset.LowPart = offsetLow;

	status = ZwWriteFile(hFile,/*文件句柄*/
						 NULL, /*事件对象,用户异步IO*/
						 NULL,/*APC例程,用于异步IO*/
						 NULL, /*APC环境*/
						 &isb,/*IO状态*/
						 pBuff,/*写入到文件中的缓冲区*/
						 sizeToWrite,/*写入的字节数*/
						 &offset,/*写入到的文件偏移*/
						 NULL);
	if (status == STATUS_SUCCESS)
		*write = isb.Information;

	return status;
}

NTSTATUS copyFile(wchar_t* srcPath,
				  wchar_t* destPath)
{
	HANDLE hSrc = (HANDLE)-1;
	HANDLE hDest = (HANDLE)-1;
	NTSTATUS status = STATUS_SUCCESS;
	ULONG64 srcSize = 0;
	ULONG	size = 0;
	char*   pBuff = NULL;
	__try
	{
		// 1. 先打开源文件
		status = createFile(srcPath,
							GENERIC_READ,
							FILE_SHARE_READ,
							FILE_OPEN_IF,
							FALSE,
							&hSrc);

		if (STATUS_SUCCESS != status)
		{
			__leave;
		}


		// 2. 获取源文件大小
		if (STATUS_SUCCESS != getFileSize(hSrc, &srcSize))
		{
			__leave;
		}

		// 3. 分配内存空间保存源文件的数据
		pBuff = ExAllocatePool(PagedPool, (ULONG)srcSize);
		if (pBuff == NULL)
		{
			__leave;
		}

		// 3. 读取源文件的数据到内存中.
		status = readFile(hSrc, 0, 0, (ULONG)srcSize, pBuff, &size);
		if (STATUS_SUCCESS != status || size != (ULONG)srcSize)
		{
			__leave;
		}

		// 4. 打开目标文件
		status = createFile(destPath,
							GENERIC_WRITE,
							FILE_SHARE_READ,
							FILE_CREATE,
							FALSE,
							&hDest);
		if (STATUS_SUCCESS != status)
		{
			__leave;
		}

		// 5. 将源文件的数据写入到目标文件
		status = writeFile(hDest, 0, 0, (ULONG)srcSize, pBuff, &size);
		if (STATUS_SUCCESS != status || size != srcSize)
		{
			__leave;
		}
		status = STATUS_SUCCESS;
	}
	__finally
	{
		// 6. 关闭源文件
		if (hSrc != (HANDLE)-1)
		{
			ZwClose(hSrc);
		}

		// 7. 关闭目标文件
		if (hDest != (HANDLE)-1)
		{
			ZwClose(hDest);
		}

		// 8. 释放缓冲区
		if (pBuff)
		{
			ExFreePool(pBuff);
		}
	}
	return status;
}

NTSTATUS moveFile(wchar_t* srcPath, wchar_t* destPath)
{
	NTSTATUS status = STATUS_SUCCESS;
	// 1. 拷贝一份文件
	status = copyFile(srcPath, destPath);

	// 2. 如果拷贝成功了,删除源文件
	if (status == STATUS_SUCCESS)
	{
		status = removeFile(srcPath);
	}
	return status;
}

NTSTATUS removeFile(wchar_t* filepath)
{

	UNICODE_STRING path;
	RtlInitUnicodeString(&path, filepath);

	// 1. 初始化OBJECT_ATTRIBUTES的内容
	OBJECT_ATTRIBUTES objAttrib = { 0 };
	ULONG             ulAttributes = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
	InitializeObjectAttributes(&objAttrib,    // 返回初始化完毕的结构体
							   &path,		  // 文件对象名称
							   ulAttributes,  // 对象属性
							   NULL,          // 根目录(一般为NULL)
							   NULL);         // 安全属性(一般为NULL)
	// 2. 删除指定文件/文件夹
	return ZwDeleteFile(&objAttrib);
}


NTSTATUS listDirGet(wchar_t* dir, FILE_BOTH_DIR_INFORMATION** fileInfo, ULONG maxFileCount)
{
	NTSTATUS status = STATUS_SUCCESS;
	IO_STATUS_BLOCK isb = { 0 };
	HANDLE		 hDir = (HANDLE)-1;
	VOID*		pBuff = NULL;
	__try{
		// 1. 打开目录
		status = createFile(dir,
							GENERIC_READ,
							FILE_SHARE_READ,
							FILE_OPEN_IF,
							TRUE,
							&hDir);
		if (STATUS_SUCCESS != status)
			__leave;

		// 计算出保存一个文件信息所需的最大字节数 = 结构体大小 + 文件名大小
		ULONG signalFileInfoSize = sizeof(FILE_BOTH_DIR_INFORMATION) + 267 * 2;
		// 计算出总空间字节数
		ULONG totalSize = signalFileInfoSize * maxFileCount;

		// 申请内存空间
		pBuff = ExAllocatePool(PagedPool, totalSize);
		if (pBuff == NULL)
			__leave;

		// 第一次调用,获取所需缓冲区字节数
		status = ZwQueryDirectoryFile(hDir, /*目录句柄*/
									  NULL, /*事件对象*/
									  NULL, /*完成通知例程*/
									  NULL, /*完成通知例程附加参数*/
									  &isb, /*IO状态*/
									  pBuff, /*输出的文件信息*/
									  totalSize,/*文件信息缓冲区的字节数*/
									  FileBothDirectoryInformation,/*获取信息的类型*/
									  FALSE,/*是否只获取第一个*/
									  0,
									  TRUE/*是否重新扫描目录*/);
		// 保存缓冲区的内容首地址.
		if (status == STATUS_SUCCESS)
			*fileInfo = (FILE_BOTH_DIR_INFORMATION*)pBuff;
	}
	__finally{

		if (hDir != (HANDLE)-1)
		{
			ZwClose(hDir);
		}
		if (status != STATUS_SUCCESS && pBuff != NULL)
		{
			ExFreePool(pBuff);
		}
	}
	return status;
}

NTSTATUS firstFile(wchar_t* dir, HANDLE* hFind, FILE_BOTH_DIR_INFORMATION* fileInfo, int size)
{
	NTSTATUS status = STATUS_SUCCESS;
	IO_STATUS_BLOCK isb = { 0 };
	// 1. 打开目录
	status = createFile(dir,
						GENERIC_READ,
						FILE_SHARE_READ,
						FILE_OPEN_IF,
						TRUE,
						hFind);
	if (STATUS_SUCCESS != status)
		return status;

	// 第一次调用,获取所需缓冲区字节数
	status = ZwQueryDirectoryFile(*hFind, /*目录句柄*/
								  NULL, /*事件对象*/
								  NULL, /*完成通知例程*/
								  NULL, /*完成通知例程附加参数*/
								  &isb, /*IO状态*/
								  fileInfo, /*输出的文件信息*/
								  size,/*文件信息缓冲区的字节数*/
								  FileBothDirectoryInformation,/*获取信息的类型*/
								  TRUE,/*是否只获取第一个*/
								  0,
								  TRUE/*是否重新扫描目录*/);
	
	return status;
}

NTSTATUS nextFile(HANDLE hFind, FILE_BOTH_DIR_INFORMATION* fileInfo, int size)
{
	IO_STATUS_BLOCK isb = { 0 };
	// 第一次调用,获取所需缓冲区字节数
	return ZwQueryDirectoryFile(hFind, /*目录句柄*/
								NULL, /*事件对象*/
								NULL, /*完成通知例程*/
								NULL, /*完成通知例程附加参数*/
								&isb, /*IO状态*/
								fileInfo, /*输出的文件信息*/
								size,/*文件信息缓冲区的字节数*/
								FileBothDirectoryInformation,/*获取信息的类型*/
								TRUE,/*是否只获取第一个*/
								0,
								FALSE/*是否重新扫描目录*/);
}

void listDirFree(FILE_BOTH_DIR_INFORMATION* fileInfo)
{
	ExFreePool(fileInfo);
}


