// CGDT.cpp: 实现文件
//

#include "pch.h"
#include "MyARK.h"
#include "CGDT.h"
#include "afxdialogex.h"
#include "Data.h"


// CGDT 对话框

IMPLEMENT_DYNAMIC(CGDT, CDialogEx)

CGDT::CGDT(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GDT, pParent)
{

}

CGDT::~CGDT()
{
}

void CGDT::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CGDT, CDialogEx)
ON_MESSAGE(WM_FLUSHGDT, &CGDT::OnFlushgdt)
ON_COMMAND(ID_GDT_32780, &CGDT::OnFlush)
ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CGDT::OnRclickList1)
END_MESSAGE_MAP()


// CGDT 消息处理程序
DWORD WINAPI ThreadProc3(_In_ LPVOID lpParameter)
{
	Sleep(300);
	HWND hWnd = (HWND)lpParameter;
	SendMessage(hWnd, WM_FLUSHGDT, 0, 0);
	return 0;
}

BOOL CGDT::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	DWORD dwOldStyle = m_ListCtrl.GetExtendedStyle();
	m_ListCtrl.SetExtendedStyle(dwOldStyle | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	CRect cRect;
	m_ListCtrl.GetClientRect(cRect);
	m_ListCtrl.InsertColumn(0, L"基址", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(1, L"界限", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(2, L"段粒度", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(3, L"段特权", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(4, L"类型", 0, cRect.Width() / 5);
	m_Menu.LoadMenuW(IDR_MENU5);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc3, this->m_hWnd, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


afx_msg LRESULT CGDT::OnFlushgdt(WPARAM wParam, LPARAM lParam)
{
	m_ListCtrl.DeleteAllItems();
	GDT_ENTRY test;
	DWORD dwSize = 0;
	DeviceIoControl(g_hDev, ENUMGDT, NULL, NULL, &test, sizeof(GDT_ENTRY), &dwSize, NULL);
	int nCount = dwSize / sizeof(GDT_ENTRY);
	PGDT_ENTRY pGdt = new GDT_ENTRY[nCount]();
	DeviceIoControl(g_hDev, ENUMGDT, NULL, NULL, pGdt, dwSize, &dwSize, NULL);
	int i = 0;
	while (nCount)
	{
		--nCount;
		m_ListCtrl.InsertItem(i, _T(""));
		CString Buffer;
		LONG Base = (pGdt[i].Base24_31 << 24) + pGdt[i].base0_23;
		Buffer.Format(L"0x%08x", Base);
		m_ListCtrl.SetItemText(i, 0, Buffer);	// 基址
		LONG Limit = (pGdt[i].Limit16_19 << 16) + pGdt[i].Limit0_15;
		Buffer.Format(L"0x%08x", Limit);
		m_ListCtrl.SetItemText(i, 1, Buffer);	// 限长
		if (pGdt[i].G == 0)
			Buffer = L"Byte";
		else if (pGdt[i].G == 1)
			Buffer = L"Page";
		m_ListCtrl.SetItemText(i, 2, Buffer);	// 粒度
		Buffer.Format(L"%d", pGdt[i].DPL);
		m_ListCtrl.SetItemText(i, 3, Buffer);	// DPL
		// 首先判断类型
		if (pGdt[i].S == 0) // 系统段
		{
			switch (pGdt[i].TYPE)
			{
			case 12:
				Buffer = L"调用门";
			case 14:
				Buffer = L"中断门";
			case 15:
				Buffer = L"陷阱门";
			case 5:
				Buffer = L"任务门";
			default:
				Buffer = L"系统段";
				break;
			}
		}
		else if (pGdt[i].S == 1)	// 数据段或者代码段
		{
			if (pGdt[i].TYPE >= 8)
				Buffer = L"代码段";
			else if (pGdt[i].TYPE < 8)
				Buffer = L"数据段";
		}
		m_ListCtrl.SetItemText(i, 4, Buffer);	// 粒度
		++i;
	}
	delete[] pGdt;
	return 0;
}


void CGDT::OnFlush()
{
	SendMessage(WM_FLUSHGDT, 0, 0);
}


void CGDT::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult)
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
