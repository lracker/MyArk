#pragma once
#include <winioctl.h>
#define WM_FLUSHDRIVER WM_USER + 1001
#define WM_FLUSHPROCESS WM_USER + 1002
#define WM_FLUSHIDT WM_USER + 1003
#define WM_FLUSHGDT WM_USER + 1004
#define WM_FLUSHSSDT WM_USER + 1005

#define MYCTLCODE(code)CTL_CODE(FILE_DEVICE_UNKNOWN,0x800+(code),METHOD_OUT_DIRECT ,FILE_ANY_ACCESS)
typedef enum _MyCtlCode
{
	ENUMDRIVER = MYCTLCODE(0),
	HIDEDRIVER = MYCTLCODE(1),
	ENUMPROCESS = MYCTLCODE(2),
	ENUMTHREAD = MYCTLCODE(3),
	ENUMMODULE = MYCTLCODE(4),
	HIDEPROCESS = MYCTLCODE(5),
	TERMINATEPROCESS = MYCTLCODE(6),
	ENUMFILE = MYCTLCODE(7),
	DELETEFILE = MYCTLCODE(8),
	ENUMIDT = MYCTLCODE(9),
	ENUMGDT = MYCTLCODE(10),
	ENUMSSDT = MYCTLCODE(11),
	GETPID = MYCTLCODE(12),
	ENUMREGISTER = MYCTLCODE(13),
	NEWREG = MYCTLCODE(14),
	DELETEREG = MYCTLCODE(15),
}MyCtlCode;

#pragma pack(1)
// 驱动结构体
typedef struct _DRIVER
{
	DWORD dwDllBase;		// 基址
	DWORD dwSize;			// 大小
	WCHAR Name[256];		// 驱动名
	WCHAR FullDllName[256]; // 路径
	struct _DRIVER* Next;	// 配合驱动模块那边的结构体需要的字节数
}DRIVER, * PDRIVER;
#pragma pack()

#pragma pack(1)
typedef struct _PROCESS
{
	DWORD dwPID;			// 进程ID
	DWORD dwPPID;			// 父进程ID
	CHAR Name[256];			// 进程名
	WCHAR FullDllName[256]; // 路径
	struct _PROCESS* Next;
}PROCESS, * PPROCESS;
#pragma pack()

#pragma pack(1)
typedef struct _THREAD
{
	DWORD dwTID;			// 线程ID
	DWORD dwBasePriority;	// 优先级
	DWORD dwStartAddress;	// 线程的起始地址
	DWORD dwStatus;			// 状态
	struct _THREAD* Next;	//
}THREAD, * PTHREAD;
#pragma pack()

#pragma pack(1)
typedef struct _MODULE
{
	WCHAR FULLDLLNAME[256];	// 模块路径
	DWORD dwStartAddress;	// 模块基地址
	DWORD dwSize;			// 模块大小
	struct _MODULE* Next;	//
}MODULE, * PMODULE;
#pragma pack()
// 全局设备名
extern HANDLE g_hDev;

#pragma pack(1)
typedef struct _ENUMFILES
{
	char FileOrDirectory;	// 一个字节来保存是文件还是目录
	WCHAR FileName[256];	// 文件名
	LONGLONG Size;			// 文件大小
	LARGE_INTEGER CreateTime;	// 创建时间
	LARGE_INTEGER ChangeTime;	// 最后修改时间
	struct _ENUMFILES* Next;
}ENUMFILES, *PENUMFILES;
#pragma pack(1)

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
	WCHAR KeyName[256];		// 项名
	WCHAR ValueName[256];   // 值的名字
	DWORD ValueType;		// 值的类型
	UCHAR Value[256];	    // 值
}REGISTER, *PREGISTER;
#pragma pack()