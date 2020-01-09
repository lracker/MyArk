// CSSDT.cpp: 实现文件
//

#include "pch.h"
#include "MyARK.h"
#include "CSSDT.h"
#include "afxdialogex.h"
#include "Data.h"


// CSSDT 对话框

IMPLEMENT_DYNAMIC(CSSDT, CDialogEx)

CSSDT::CSSDT(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SSDT, pParent)
{

}

CSSDT::~CSSDT()
{
}

void CSSDT::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CSSDT, CDialogEx)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CSSDT::OnRclickList1)
	ON_COMMAND(ID_SSDT_32781, &CSSDT::OnFlushSSDT)
	ON_MESSAGE(WM_FLUSHSSDT, &CSSDT::OnFlushssdt)
END_MESSAGE_MAP()


// CSSDT 消息处理程序
DWORD WINAPI ThreadProc4(_In_ LPVOID lpParameter)
{
	Sleep(300);
	HWND hWnd = (HWND)lpParameter;
	SendMessage(hWnd, WM_FLUSHSSDT, 0, 0);
	return 0;
}

BOOL CSSDT::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	DWORD dwOldStyle = 0;
	dwOldStyle = m_ListCtrl.GetExtendedStyle();
	m_ListCtrl.SetExtendedStyle(dwOldStyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect cRect;
	m_ListCtrl.GetClientRect(cRect);
	m_ListCtrl.InsertColumn(0, L"系统调用号", 0, cRect.Width() / 2);
	m_ListCtrl.InsertColumn(1, L"地址", 0, cRect.Width() / 2);
	m_Menu.LoadMenuW(IDR_MENU6);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc4, this->m_hWnd, 0, 0);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CSSDT::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	POINT Point = { 0 };
	GetCursorPos(&Point);
	// 获取菜单的子菜单
	CMenu* SubMenu = m_Menu.GetSubMenu(0);
	*pResult = 0;
}


void CSSDT::OnFlushSSDT()
{
	SendMessage(WM_FLUSHSSDT, 0, 0);
}

afx_msg LRESULT CSSDT::OnFlushssdt(WPARAM wParam, LPARAM lParam)
{
	SSDT test;
	DWORD dwSize = 0;
	DeviceIoControl(g_hDev, ENUMSSDT, NULL, NULL, &test, sizeof(SSDT), &dwSize, NULL);
	int nNum = dwSize / sizeof(SSDT);
	PSSDT pSSDT = new SSDT[nNum]();
	DeviceIoControl(g_hDev, ENUMSSDT, NULL, NULL, pSSDT, dwSize, &dwSize, NULL);
	CString Buffer;
	for (int i = 0; i < nNum; ++i)
	{
		m_ListCtrl.InsertItem(i, _T(""));
		Buffer.Format(L"0x%x", pSSDT[i].SysCallIndex);
		m_ListCtrl.SetItemText(i, 0, Buffer);
		Buffer.Format(L"0x%08x", pSSDT[i].Address);
		m_ListCtrl.SetItemText(i, 1, Buffer);
	}
	return 0;
}
