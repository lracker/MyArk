
// MyARKDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MyARK.h"
#include "MyARKDlg.h"
#include "afxdialogex.h"
#include "CDriver.h"
#include "CProcess.h"
#include "CEnumFile.h"
#include "CIDT.h"
#include "CGDT.h"
#include "Data.h"
#include "CSSDT.h"
#include <winsvc.h>
#include "CRegister.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyARKDlg 对话框



CMyARKDlg::CMyARKDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MYARK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyARKDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabCtrl);
}

BEGIN_MESSAGE_MAP(CMyARKDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMyARKDlg::OnSelchangeTab1)
END_MESSAGE_MAP()


// CMyARKDlg 消息处理程序

BOOL CMyARKDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	// 加载驱动
	LoadDriver();
	// 加载
	StartDriver();
	// 打开设备对象
	g_hDev = CreateFile(L"\\??\\Device_001", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (g_hDev == INVALID_HANDLE_VALUE)
	{
		CString Buffer;
		Buffer.Format(L"[3环程序]打开设备失败: %d\n", GetLastError());
		MessageBox(Buffer);
	}
	// 发送PID 
	int PID = _getpid();
	DWORD dwSize = 0;
	DeviceIoControl(g_hDev, GETPID, &PID, sizeof(int), NULL, NULL, &dwSize, NULL);

	// 初始化Tab控件
	m_TabCtrl.InsertItem(0, L"驱动");
	m_TabCtrl.InsertItem(1, L"进程");
	m_TabCtrl.InsertItem(2, L"文件");
	m_TabCtrl.InsertItem(3, L"IDT");
	m_TabCtrl.InsertItem(4, L"GDT");
	m_TabCtrl.InsertItem(5, L"SSDT");
	m_TabCtrl.InsertItem(6, L"注册表");
	m_TabCtrl.m_Dia[0] = new CDriver();
	m_TabCtrl.m_Dia[1] = new CProcess();
	m_TabCtrl.m_Dia[2] = new CEnumFile();
	m_TabCtrl.m_Dia[3] = new CIDT();
	m_TabCtrl.m_Dia[4] = new CGDT();
	m_TabCtrl.m_Dia[5] = new CSSDT();
	m_TabCtrl.m_Dia[6] = new CRegister();
	m_TabCtrl.m_Dia[0]->Create(IDD_DRIVER, &m_TabCtrl);
	m_TabCtrl.m_Dia[1]->Create(IDD_PROCESS, &m_TabCtrl);
	m_TabCtrl.m_Dia[2]->Create(IDD_ENUMFILE, &m_TabCtrl);
	m_TabCtrl.m_Dia[3]->Create(IDD_IDT, &m_TabCtrl);
	m_TabCtrl.m_Dia[4]->Create(IDD_GDT, &m_TabCtrl);
	m_TabCtrl.m_Dia[5]->Create(IDD_SSDT, &m_TabCtrl);
	m_TabCtrl.m_Dia[6]->Create(IDD_REGISTER, &m_TabCtrl);
	CRect pos = {};
	m_TabCtrl.GetClientRect(pos);		//转换为客户端坐标
	pos.DeflateRect(2, 30, 3, 2);	// 移动坐标，免得盖住菜单了
	// 移动窗口
	m_TabCtrl.m_Dia[0]->MoveWindow(pos);
	m_TabCtrl.m_Dia[0]->ShowWindow(SW_SHOW);
	m_TabCtrl.m_Dia[1]->ShowWindow(SW_HIDE);
	m_TabCtrl.m_Dia[2]->ShowWindow(SW_HIDE);
	m_TabCtrl.m_Dia[3]->ShowWindow(SW_HIDE);
	m_TabCtrl.m_Dia[4]->ShowWindow(SW_HIDE);
	m_TabCtrl.m_Dia[5]->ShowWindow(SW_HIDE);
	m_TabCtrl.m_Dia[6]->ShowWindow(SW_HIDE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyARKDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMyARKDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//*****************************************************************************************
// 函数名称: OnSelchangeTab1
// 函数说明: TAB控件
// 作    者: lracker
// 时    间: 2019/12/25
// 参    数: NMHDR *
// 参    数: LRESULT *
// 返 回 值: void
//*****************************************************************************************
void CMyARKDlg::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	CRect cRect;
	GetClientRect(cRect);
	cRect.DeflateRect(2, 30, 3, 2);
	// 获取所选TAB项
	DWORD dwSel = m_TabCtrl.GetCurSel();
	for (DWORD i = 0; i < m_TabCtrl.m_dwTabNum; ++i)
	{
		if (dwSel == i)
		{
			m_TabCtrl.m_Dia[i]->MoveWindow(cRect);
			m_TabCtrl.m_Dia[i]->ShowWindow(SW_SHOW);
		}
		else
		{
			m_TabCtrl.m_Dia[i]->ShowWindow(SW_HIDE);
		}
	}
	*pResult = 0;
}

//*****************************************************************************************
// 函数名称: LoadDriver
// 函数说明: 加载驱动
// 作    者: lracker
// 时    间: 2019/12/27
// 返 回 值: VOID
//*****************************************************************************************
VOID CMyARKDlg::LoadDriver()
{
	TCHAR FilePath[MAX_PATH] = L"C:\\Users\\15pb-win7\\Desktop\\MyARKDriver.sys";
	// 打开服务管理器
	m_hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	// 创建服务
	m_hService = CreateService(m_hSCManager, L"MyDriver1", L"MyDriver1", SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE, FilePath, NULL, NULL, NULL, NULL, NULL);
	m_dwLastError = GetLastError();
}

//*****************************************************************************************
// 函数名称: StartDriver
// 函数说明: 启动驱动
// 作    者: lracker
// 时    间: 2019/12/27
// 返 回 值: VOID
//*****************************************************************************************
VOID CMyARKDlg::StartDriver()
{
	// 如果服务存在，只要打开就好了
	if (m_dwLastError == ERROR_SERVICE_EXISTS)
	{
		m_hService = OpenService(m_hService, L"MyDriver", SERVICE_ALL_ACCESS);
	}
	// 创建服务是否成功
	if (!m_hService)
	{
		CloseServiceHandle(m_hSCManager);
		return;
	}
	// 启动服务
	SERVICE_STATUS status;
	// 查询服务状态
	QueryServiceStatus(m_hService, &status);
	// 服务处于暂停状态
	if (status.dwCurrentState == SERVICE_STOPPED)
	{
		// 启动服务
		StartService(m_hService, NULL, NULL);
		Sleep(1000);
		// 再检查服务状态
		QueryServiceStatus(m_hService, &status);
		// 服务是否处于运行状态
		if (status.dwCurrentState != SERVICE_RUNNING)
		{
			MessageBox(L"启动失败");
			CloseServiceHandle(m_hSCManager);
			CloseServiceHandle(m_hService);
		}
	}
}


//*****************************************************************************************
// 函数名称: StopDriver
// 函数说明: 停止驱动
// 作    者: lracker
// 时    间: 2019/12/27
// 返 回 值: VOID
//*****************************************************************************************
VOID CMyARKDlg::StopDriver()
{
	CloseHandle(g_hDev);
	// 停止服务
	SERVICE_STATUS status;
	// 检查服务状态
	BOOL bRet = QueryServiceStatus(m_hService, &status);
	// 如果不处于暂停状态
	if (status.dwCurrentState != SERVICE_STOPPED)
	{
		// 停止服务
		ControlService(m_hService, SERVICE_CONTROL_STOP, &status);
		// 直到服务停止
		while (QueryServiceStatus(m_hService, &status) == TRUE)
		{
			Sleep(status.dwWaitHint);
			break;
		}
	}
}

//*****************************************************************************************
// 函数名称: UnLoadDriver
// 函数说明: 卸载驱动
// 作    者: lracker
// 时    间: 2019/12/27
// 返 回 值: VOID
//*****************************************************************************************
VOID CMyARKDlg::UnLoadDriver()
{
	if (!DeleteService(m_hService))
	{
		CString cs;
		cs.Format(_T("%d"), GetLastError());
		MessageBox(L"DeleteService Failed", cs);
	}
	// 关闭服务管理器句柄
	CloseServiceHandle(m_hService);
	CloseServiceHandle(m_hSCManager);
}

//*****************************************************************************************
// 函数名称: DestroyWindow
// 函数说明: 关闭窗口的时候调用
// 作    者: lracker
// 时    间: 2019/12/27
// 返 回 值: BOOL
//*****************************************************************************************
BOOL CMyARKDlg::DestroyWindow()
{
	// 停止服务
	StopDriver();
	// 卸载驱动
	UnLoadDriver();
	return CDialogEx::DestroyWindow();
}
