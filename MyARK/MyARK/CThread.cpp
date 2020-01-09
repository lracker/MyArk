// CThread.cpp: 实现文件
//

#include "pch.h"
#include "MyARK.h"
#include "CThread.h"
#include "afxdialogex.h"
#include "Data.h"


// CThread 对话框

IMPLEMENT_DYNAMIC(CThread, CDialogEx)

CThread::CThread(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_THREAD, pParent)
{

}

CThread::~CThread()
{
}

void CThread::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CThread, CDialogEx)
END_MESSAGE_MAP()


// CThread 消息处理程序


BOOL CThread::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	DWORD dwOldStyle = 0;
	dwOldStyle = m_ListCtrl.GetExtendedStyle();
	m_ListCtrl.SetExtendedStyle(dwOldStyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect cRect;
	m_ListCtrl.GetClientRect(cRect);
	m_ListCtrl.InsertColumn(0, L"序号", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(1, L"TID", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(2, L"优先级", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(3, L"线程起始地址", 0, cRect.Width() / 5);
	m_ListCtrl.InsertColumn(4, L"状态", 0, cRect.Width() / 5);
	GetThread();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

WCHAR* NumToStatus(INT nNum)
{
	WCHAR* Buffer = L"NULL";
	switch (nNum)
	{
	case 0:
		Buffer = L"已初始化";
		break;
	case 1:
		Buffer = L"准备";
		break;
	case 2:
		Buffer = L"运行中";
		break;
	case 3:
		Buffer = L"Standby";
		break;
	case 4:
		Buffer = L"终止";
		break;
	case 5:
		Buffer = L"等待";
		break;
	case 6:
		Buffer = L"Transition";
		break;
	case 7:
		Buffer = L"DeferredReady";
		break;
	case 8:
		Buffer = L"GateWait";
		break;
	default:
		break;
	}
	return Buffer;
}

VOID CThread::GetThread()
{
	m_ListCtrl.DeleteAllItems();
	PTHREAD test;
	DWORD dwSize = 0;
	// 先发出一次请求，返回所需要的大小。
	DeviceIoControl(g_hDev, ENUMTHREAD, &m_dwPID, sizeof(DWORD), &test, sizeof(THREAD), &dwSize, NULL);
	// 根据返回来的大小重新申请空间
	PTHREAD pThread = new THREAD[dwSize]();
	DeviceIoControl(g_hDev, ENUMTHREAD, &m_dwPID, dwSize, pThread, dwSize, &dwSize, NULL);
	// 所有项数
	int nCount = dwSize / sizeof(THREAD);
	int i = 0;
	int nIndex = 0;
	while (nCount)
	{
		--nCount;
		m_ListCtrl.InsertItem(nIndex, _T(""));
		CString Temp;
		Temp.Format(L"%d", nIndex + 1);
		m_ListCtrl.SetItemText(nIndex, 0, Temp);				// 序号
		Temp.Format(L"%d", pThread[i].dwTID);
		m_ListCtrl.SetItemText(nIndex, 1, Temp);				// TID
		Temp.Format(L"%d", pThread[i].dwBasePriority);		
		m_ListCtrl.SetItemText(nIndex, 2, Temp);				// BasePriority
		Temp.Format(L"0x%08x", pThread[i].dwStartAddress);		
		m_ListCtrl.SetItemText(nIndex, 3, Temp);				// 起始地址
		m_ListCtrl.SetItemText(nIndex, 4, NumToStatus(pThread[i].dwStatus));	// 状态
		++i;
		++nIndex;
	}
	delete[] pThread;
}

