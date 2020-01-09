// CModule.cpp: 实现文件
//

#include "pch.h"
#include "MyARK.h"
#include "CModule.h"
#include "afxdialogex.h"
#include "Data.h"


// CModule 对话框

IMPLEMENT_DYNAMIC(CModule, CDialogEx)

CModule::CModule(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MODULE, pParent)
{

}

CModule::~CModule()
{
}

void CModule::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CModule, CDialogEx)
END_MESSAGE_MAP()


// CModule 消息处理程序


//*****************************************************************************************
// 函数名称: GetModule
// 函数说明: 获取模块的
// 作    者: lracker
// 时    间: 2019/12/25
// 返 回 值: VOID
//*****************************************************************************************
VOID CModule::GetModule()
{
	m_ListCtrl.DeleteAllItems();
	PMODULE test;
	DWORD dwSize = 0;
	// 先发出一次请求，返回所需要的大小。
	DeviceIoControl(g_hDev, ENUMMODULE, &m_dwPID, sizeof(DWORD), &test, sizeof(MODULE), &dwSize, NULL);
	// 根据返回来的大小重新申请空间
	PMODULE pModule = new MODULE[dwSize]();
	DeviceIoControl(g_hDev, ENUMMODULE, &m_dwPID, dwSize, pModule, dwSize, &dwSize, NULL);
	// 所有项数
	int nCount = dwSize / sizeof(MODULE);
	int i = 0;
	int nIndex = 0;
	while (nCount)
	{
		--nCount;
		m_ListCtrl.InsertItem(nIndex, _T(""));
		CString Temp;
		Temp.Format(L"%d", nIndex + 1);
		m_ListCtrl.SetItemText(nIndex, 0, Temp);					// 序号
		m_ListCtrl.SetItemText(nIndex, 1, pModule[i].FULLDLLNAME);	// 路径
		Temp.Format(L"0x%08x", pModule[i].dwStartAddress);
		m_ListCtrl.SetItemText(nIndex, 2, Temp);					// 基地址
		Temp.Format(L"0x%08x", pModule[i].dwSize);
		m_ListCtrl.SetItemText(nIndex, 3, Temp);					// 大小
		++i;
		++nIndex;
	}
	delete[] pModule;
}

BOOL CModule::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	DWORD dwStyle = 0;
	dwStyle = m_ListCtrl.GetExtendedStyle();
	m_ListCtrl.SetExtendedStyle(dwStyle | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	CRect cRect;
	m_ListCtrl.GetClientRect(cRect);
	m_ListCtrl.InsertColumn(0, L"序号", 0, cRect.Width() / 4);
	m_ListCtrl.InsertColumn(1, L"模块路径", 0, cRect.Width() / 4);
	m_ListCtrl.InsertColumn(2, L"基地址", 0, cRect.Width() / 4);
	m_ListCtrl.InsertColumn(3, L"大小", 0, cRect.Width() / 4);
	GetModule();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

