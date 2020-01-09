// CIDT.cpp: 实现文件
//

#include "pch.h"
#include "MyARK.h"
#include "CIDT.h"
#include "afxdialogex.h"
#include "Data.h"


// CIDT 对话框

IMPLEMENT_DYNAMIC(CIDT, CDialogEx)

CIDT::CIDT(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IDT, pParent)
{

}

CIDT::~CIDT()
{
}

void CIDT::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CIDT, CDialogEx)
	ON_MESSAGE(WM_FLUSHIDT, &CIDT::OnFlushIDT)
	ON_COMMAND(ID_IDT_32779, &CIDT::OnFlush)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CIDT::OnRclickList1)
END_MESSAGE_MAP()


// CIDT 消息处理程序

DWORD WINAPI ThreadProc2(_In_ LPVOID lpParameter)
{
	Sleep(300);
	HWND hWnd = (HWND)lpParameter;
	SendMessage(hWnd, WM_FLUSHIDT, 0, 0);
	return 0;
}

BOOL CIDT::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	DWORD dwOldStyle = m_ListCtrl.GetExtendedStyle();
	m_ListCtrl.SetExtendedStyle(dwOldStyle | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	CRect cRect;
	m_ListCtrl.GetClientRect(cRect);
	m_ListCtrl.InsertColumn(0, L"中断地址", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(1, L"中断号", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(2, L"段选择子", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(3, L"类型", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(4, L"特权等级", 0, cRect.Width() / 5);
	m_Menu.LoadMenuW(IDR_MENU4);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc2, this->m_hWnd, 0, 0);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


afx_msg LRESULT CIDT::OnFlushIDT(WPARAM wParam, LPARAM lParam)
{
	m_ListCtrl.DeleteAllItems();
	PIDT_ENTRY pIdt = new IDT_ENTRY[0x100]();
	DWORD dwSize = 0;
	DeviceIoControl(g_hDev, ENUMIDT, NULL, NULL, pIdt, 0x100 * sizeof(IDT_ENTRY), &dwSize, NULL);
	for (int i = 0; i < 0x100; ++i)
	{
		m_ListCtrl.InsertItem(i, _T(""));
		CString Temp;
		ULONG Idt_address = MAKELONG(pIdt[i].uOffsetLow, pIdt[i].uOffsetHigh);
		Temp.Format(L"0x%08x", Idt_address);
		m_ListCtrl.SetItemText(i, 0, Temp);	// 中断地址
		Temp.Format(L"%d", i);
		m_ListCtrl.SetItemText(i, 1, Temp);	// 中断号
		Temp.Format(L"%d", pIdt[i].uSelector);
		m_ListCtrl.SetItemText(i, 2, Temp);	// 段选择子
		Temp.Format(L"%d", pIdt[i].uType);
		m_ListCtrl.SetItemText(i, 3, Temp);	// 类型
		Temp.Format(L"%d", pIdt[i].uDpl);	
		m_ListCtrl.SetItemText(i, 4, Temp);	// 特权等级
	}
	return 0;
}


void CIDT::OnFlush()
{
	SendMessage(WM_FLUSHIDT, 0, 0);
}


void CIDT::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// 获取当前鼠标相对于屏幕的位置
	POINT Point = { 0 };
	GetCursorPos(&Point);
	// 获取菜单的子菜单
	CMenu* SubMenu = m_Menu.GetSubMenu(0);
	// 弹出窗口
	SubMenu->TrackPopupMenu(TPM_LEFTALIGN, Point.x, Point.y, this);
	*pResult = 0;
}
