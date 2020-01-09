#pragma once
#include <ntdef.h>
// 驱动结构体
#pragma pack(1)
typedef struct _DRIVER
{
	ULONG DllBase;				// 基址
	ULONG Size;					// 大小
	char Name[256 * 2];			// 驱动名
	char FullDllName[256 * 2];  // 路径
	struct _DRIVER* Next;
}DRIVER, * PDRIVER;
#pragma pack()

#pragma pack(1)
typedef struct _PROCESS
{
	ULONG dwPID;			   // 进程ID
	ULONG dwPPID;			   // 父进程ID
	char Name[256];		       // 进程名
	char FullDllName[256 * 2]; // 路径
	struct _PROCESS* Next;
}PROCESS, * PPROCESS;
#pragma pack()

#pragma pack(1)
typedef struct _THREAD
{
	ULONG ulTID;			// 线程ID
	ULONG ulBasePriority;	// 优先级
	ULONG ulStartAddress;	// 线程的起始地址
	ULONG ulStatus;			// 状态
	struct _THREAD* Next;	//
}THREAD, * PTHREAD;
#pragma pack()

#pragma pack(1)
typedef struct _MODULE
{
	char FullDllName[256 * 2];	// 模块路径
	ULONG dwStartAddress;	// 模块基地址
	ULONG dwSize;			// 模块大小
	struct _MODULE* Next;	//
}MODULE, * PMODULE;
#pragma pack()
struct _PEB
{
	UCHAR InheritedAddressSpace;                                            //0x0
	UCHAR ReadImageFileExecOptions;                                         //0x1
	UCHAR BeingDebugged;
	UCHAR BitField;
	void* Mutant;                                                           //0x4
	void* ImageBaseAddress;                                                 //0x8
	struct _PEB_LDR_DATA* Ldr;
};
//0x30 bytes (sizeof)
struct _PEB_LDR_DATA
{
	ULONG Length;                                                           //0x0
	UCHAR Initialized;                                                      //0x4
	VOID* SsHandle;                                                         //0x8
	struct _LIST_ENTRY InLoadOrderModuleList;                               //0xc
	struct _LIST_ENTRY InMemoryOrderModuleList;                             //0x14
	struct _LIST_ENTRY InInitializationOrderModuleList;                     //0x1c
	VOID* EntryInProgress;                                                  //0x24
	UCHAR ShutdownInProgress;                                               //0x28
	VOID* ShutdownThreadId;                                                 //0x2c
};

typedef struct _LDR_DATA_TABLE_ENTRY {
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
	union {
		LIST_ENTRY HashLinks;
		struct {
			PVOID SectionPointer;
			ULONG CheckSum;
		}s1;
	}u1;
	union {
		struct {
			ULONG TimeDateStamp;
		}s2;
		struct {
			PVOID LoadedImports;
		}s3;
	}u2;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

#pragma pack(1)
typedef struct _ENUMFILES
{
	char FileOrDirectory;	// 一个字节来保存是文件还是目录
	char FileName[256 * 2];	// 文件名
	LONGLONG Size;			// 文件大小
	LARGE_INTEGER CreateTime;	// 创建时间
	LARGE_INTEGER ChangeTime;	// 最后修改时间
	struct _ENUMFILES* Next;
}ENUMFILES, * PENUMFILES;
#pragma pack()

#pragma pack(1)
typedef struct _IDT_ENTRY
{
	UINT16 uOffsetLow;			// 程序低地址
	UINT16 uSelector;			// 段选择子
	UINT8  uReverse_1;			// 保留
	UINT8  uType : 4;			// 中断类型
	UINT8  StorageSegment : 1;  // 为0是中断门
	UINT8  uDpl : 2;			// 特权级
	UINT8  uPresent : 1;		// 如果未使用中断，可置为0
	UINT16 uOffsetHigh;			// 处理程序高地址偏移
}IDT_ENTRY, * PIDT_ENTRY;
#pragma pack()

typedef struct _IDT_INFO
{
	UINT16 uIdtLimit;		// IDT范围
	UINT16 uLowIdtBase;		// IDT低基址
	UINT16 uHighIdtBase;	// IDT高基址
}IDT_INFO, *PIDT_INFO;

#pragma pack(1)
typedef struct _GDT_ENTRY
{
	UINT64 Limit0_15 : 16;	// 界限
	UINT64 base0_23 : 24;	// 基址
	UINT64 TYPE : 4;		// 类型
	UINT64 S : 1;			// S 位置(为0则是系统段，1是代码段或者数据段)
	UINT64 DPL : 2;			// DPL 特权级
	UINT64 P : 1;			// P位置 段存在
 	UINT64 Limit16_19 : 4;	// 界限
	UINT64 AVL : 1;			// AVL 系统软件可用位
	UINT64 O : 1;			// O
	UINT64 D_B : 1;			// D_B 默认大小
	UINT64 G : 1;			// G 颗粒度
	UINT64 Base24_31 : 8;	// Base
}GDT_ENTRY, * PGDT_ENTRY;
#pragma pack()

typedef struct _GDT_INFO
{
	UINT16 uGdtlimit;		// GDT范围
	UINT16 uLowGdtBase;		// GDT低基址
	UINT16 uHighGdtBase;	// GDT高基址
}GDT_INFO, *PGDT_INFO;


typedef struct _SystemServiceDescriptorTable
{
	PVOID ServiceTableBase;
	PULONG ServiceCounterTableBase;
	ULONG NumberOfService;
	ULONG ParamTableBase;
}SystemServiceDescriptorTable, * PSystemServiceDescriptorTable;


#pragma pack(1)
typedef struct _SSDT
{
	ULONG Address;			// 加载基址
	ULONG SysCallIndex;		// 调用号
}SSDT, * PSSDT;
#pragma pack()

#pragma pack(1)
typedef struct _REGISTER
{
	ULONG Type;				// 类型，0 为子项，1为值
	char KeyName[256 * 2];	// 项名
	char ValueName[256 * 2];// 值的名字
	ULONG ValueType;		// 值的类型
	UCHAR Value[256];			// 值
}REGISTER, * PREGISTER;
#pragma pack()