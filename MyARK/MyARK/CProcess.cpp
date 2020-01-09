// CProcess.cpp: 实现文件
//

#include "pch.h"
#include "MyARK.h"
#include "CProcess.h"
#include "afxdialogex.h"
#include "Data.h"
#include "CThread.h"
#include "CModule.h"

// CProcess 对话框

IMPLEMENT_DYNAMIC(CProcess, CDialogEx)

CProcess::CProcess(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROCESS, pParent)
{

}

CProcess::~CProcess()
{
}

void CProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CProcess, CDialogEx)
	ON_MESSAGE(WM_FLUSHPROCESS, &CProcess::OnFlushProcess)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CProcess::OnRclickList1)
	ON_COMMAND(ID_32773, &CProcess::OnFlushList)
	ON_COMMAND(ID_32774, &CProcess::OnThread)
	ON_COMMAND(ID_32775, &CProcess::OnModule)
	ON_COMMAND(ID_32776, &CProcess::OnHideProcess)
	ON_COMMAND(ID_32777, &CProcess::OnTerminateProcess)
END_MESSAGE_MAP()


DWORD WINAPI ThreadProc1(_In_ LPVOID lpParameter)
{
	Sleep(300);
	HWND hWnd = (HWND)lpParameter;
	SendMessage(hWnd, WM_FLUSHPROCESS, 0, 0);
	return 0;
}

// CProcess 消息处理程序


BOOL CProcess::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_Menu.LoadMenuW(IDR_MENU2);
	DWORD dwOldProtect = 0;
	dwOldProtect = m_ListCtrl.GetExtendedStyle();
	m_ListCtrl.SetExtendedStyle(dwOldProtect | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	CRect cRect;
	m_ListCtrl.GetClientRect(cRect);
	m_ListCtrl.InsertColumn(0, L"序号", 0, 50);
	m_ListCtrl.InsertColumn(1, L"映像名称", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(2, L"进程ID", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(3, L"父进程ID", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(4, L"映像路径", 0, 400);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc1, this->m_hWnd, 0, 0);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


//*****************************************************************************************
// 函数名称: OnFlush
// 函数说明: 刷新列表
// 作    者: lracker
// 时    间: 2019/12/24
// 参    数: WPARAM
// 参    数: LPARAM
// 返 回 值: LRESULT
//*****************************************************************************************
afx_msg LRESULT CProcess::OnFlushProcess(WPARAM wParam, LPARAM lParam)
{
	m_ListCtrl.DeleteAllItems();
	PROCESS test;
	DWORD dwSize = 0;
	// 先发出一次请求，返回所需要的大小。
	DeviceIoControl(g_hDev, ENUMPROCESS, NULL, NULL, &test, sizeof(PROCESS), &dwSize, NULL);
	// 根据返回来的大小重新申请空间
	PPROCESS pProcess = new PROCESS[dwSize]();
	DeviceIoControl(g_hDev, ENUMPROCESS, pProcess, dwSize, pProcess, dwSize, &dwSize, NULL);
	// 所有项数
	int nCount = dwSize / sizeof(PROCESS);
	int i = 0;
	int nIndex = 0;
	while (nCount)
	{
		--nCount;
		USES_CONVERSION;
		CString Buffer = A2W(pProcess[i].Name);
		if (Buffer.IsEmpty())
		{
			++i;
			continue;
		}
		m_ListCtrl.InsertItem(nIndex, _T(""));
		CString Temp;
		Temp.Format(L"%d", nIndex + 1);
		m_ListCtrl.SetItemText(nIndex, 0, Temp);
		m_ListCtrl.SetItemText(nIndex, 1, Buffer);
		Temp.Format(L"%d", pProcess[i].dwPID);
		m_ListCtrl.SetItemText(nIndex, 2, Temp);
		Temp.Format(L"%d", pProcess[i].dwPPID);
		m_ListCtrl.SetItemText(nIndex, 3, Temp);
		if(Buffer == L"System")
			m_ListCtrl.SetItemText(nIndex, 4, L"System");
		else
			m_ListCtrl.SetItemText(nIndex, 4, pProcess[i].FullDllName);
		++i;
		++nIndex;
	}
	delete[] pProcess;
	return 0;
}


//*****************************************************************************************
// 函数名称: OnRclickList1
// 函数说明: 右键弹窗
// 作    者: lracker
// 时    间: 2019/12/24
// 参    数: NMHDR *
// 参    数: LRESULT *
// 返 回 值: void
//*****************************************************************************************
void CProcess::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CString Buffer = m_ListCtrl.GetItemText(pNMItemActivate->iItem, 2);
	m_dwPID = _wtoi(Buffer.GetBuffer());
	// 获取当前鼠标相对于屏幕的位置
	POINT Point = { 0 };
	GetCursorPos(&Point);
	// 获取菜单的子菜单
	CMenu* SubMenu = m_Menu.GetSubMenu(0);
	// 弹出窗口
	SubMenu->TrackPopupMenu(TPM_LEFTALIGN, Point.x, Point.y, this);
	*pResult = 0;
}


//*****************************************************************************************
// 函数名称: OnFlushList
// 函数说明: 刷新列表
// 作    者: lracker
// 时    间: 2019/12/24
// 返 回 值: void
//*****************************************************************************************
void CProcess::OnFlushList()
{
	SendMessage(WM_FLUSHPROCESS, 0, 0);
}


//*****************************************************************************************
// 函数名称: OnThread
// 函数说明: 查看进程
// 作    者: lracker
// 时    间: 2019/12/24
// 返 回 值: void
//*****************************************************************************************
void CProcess::OnThread()
{
	CThread obj;
	obj.m_dwPID = m_dwPID;
	obj.DoModal();
}


//*****************************************************************************************
// 函数名称: OnModule
// 函数说明: 查看模块
// 作    者: lracker
// 时    间: 2019/12/25
// 返 回 值: void
//*****************************************************************************************
void CProcess::OnModule()
{
	CModule obj;
	obj.m_dwPID = m_dwPID;
	obj.DoModal();
}



//*****************************************************************************************
// 函数名称: OnHideProcess
// 函数说明: 隐藏进程
// 作    者: lracker
// 时    间: 2019/12/25
// 返 回 值: void
//*****************************************************************************************
void CProcess::OnHideProcess()
{
	// 发出隐藏进程的命令
	DWORD dwSize = 0;
	DeviceIoControl(g_hDev, HIDEPROCESS, &m_dwPID, sizeof(DWORD), NULL, NULL, &dwSize, NULL);
}


//*****************************************************************************************
// 函数名称: OnTerminateProcess
// 函数说明: 结束进程
// 作    者: lracker
// 时    间: 2019/12/25
// 返 回 值: void
//*****************************************************************************************
void CProcess::OnTerminateProcess()
{
	// 发出结束进程的命令
	DWORD dwSize = 0;
	DeviceIoControl(g_hDev, TERMINATEPROCESS, &m_dwPID, sizeof(DWORD), NULL, NULL, &dwSize, NULL);
}
