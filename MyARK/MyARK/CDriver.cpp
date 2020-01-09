// CDriver.cpp: 实现文件
//

#include "pch.h"
#include "MyARK.h"
#include "CDriver.h"
#include "afxdialogex.h"
#include "Data.h"


// CDriver 对话框

IMPLEMENT_DYNAMIC(CDriver, CDialogEx)

CDriver::CDriver(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DRIVER, pParent)
{

}

CDriver::~CDriver()
{
}

void CDriver::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}

BEGIN_MESSAGE_MAP(CDriver, CDialogEx)
	ON_MESSAGE(WM_FLUSHDRIVER, &CDriver::OnFlushDriver)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CDriver::OnRclickList1)
	ON_COMMAND(ID_32771, &CDriver::OnFLUSHLIST)
	ON_COMMAND(ID_32772, &CDriver::OnHideDriver)
END_MESSAGE_MAP()


// CDriver 消息处理程序

afx_msg LRESULT CDriver::OnFlushDriver(WPARAM wParam, LPARAM lParam)
{
	m_ListCtrl.DeleteAllItems();
	DRIVER test;
	DWORD dwSize = 0;
	// 先发出一次请求，返回所需要的大小。
	DeviceIoControl(g_hDev, ENUMDRIVER, NULL, NULL, &test, sizeof(DRIVER), &dwSize, NULL);
	// 根据返回来的大小重新申请空间
	PDRIVER pDriver = new DRIVER[dwSize]();
	DeviceIoControl(g_hDev, ENUMDRIVER, pDriver, dwSize, pDriver, dwSize, &dwSize, NULL);
	// 所有项数
	int nCount = dwSize / sizeof(DRIVER);
	int i = 0;
	int nIndex = 0;
	while (nCount)
	{
		--nCount;
		CString Buffer = pDriver[i].Name;
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
		Temp.Format(L"0x%08x", pDriver[i].dwDllBase);
		m_ListCtrl.SetItemText(nIndex, 2, Temp);
		Temp.Format(L"0x%08x", pDriver[i].dwSize);
		m_ListCtrl.SetItemText(nIndex, 3, Temp);
		m_ListCtrl.SetItemText(nIndex, 4, pDriver[i].FullDllName);
		++i;
		++nIndex;
	}
	delete[] pDriver;
	return 0;
}
DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{
	Sleep(300);
	HWND hWnd = (HWND)lpParameter;
	SendMessage(hWnd,WM_FLUSHDRIVER, 0, 0);
	return 0;
}

BOOL CDriver::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 初始化列表控件
	DWORD dwOldStyle = m_ListCtrl.GetExtendedStyle();
	m_ListCtrl.SetExtendedStyle(dwOldStyle| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect cRect;
	m_ListCtrl.GetClientRect(cRect);
	m_ListCtrl.InsertColumn(0, L"序号", 0, 50);
	m_ListCtrl.InsertColumn(1, L"驱动名", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(2, L"基址", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(3, L"大小", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(4, L"路径", 0, 400);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, this->m_hWnd, 0, 0);
	m_Menu.LoadMenuW(IDR_MENU1);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}




void CDriver::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CString Buffer = m_ListCtrl.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);
	wcscpy_s(m_HideDriverName, 256, Buffer.GetBuffer());
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
// 函数名称: OnFLUSHLIST
// 函数说明: 右键刷新
// 作    者: lracker
// 时    间: 2019/12/24
// 返 回 值: void
//*****************************************************************************************
void CDriver::OnFLUSHLIST()
{
	SendMessage(WM_FLUSHDRIVER, 0, 0);
}


//*****************************************************************************************
// 函数名称: OnHide
// 函数说明: 隐藏驱动
// 作    者: lracker
// 时    间: 2019/12/24
// 返 回 值: void
//*****************************************************************************************
void CDriver::OnHideDriver()
{
	DWORD dwSize = 0;
	DeviceIoControl(g_hDev, HIDEDRIVER, m_HideDriverName, 256, NULL, NULL, &dwSize, NULL);
}

