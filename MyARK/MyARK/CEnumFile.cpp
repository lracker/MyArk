// CEnumFile.cpp: 实现文件
//

#include "pch.h"
#include "MyARK.h"
#include "CEnumFile.h"
#include "afxdialogex.h"
#include "Data.h"
#include <strsafe.h>


// CEnumFile 对话框

IMPLEMENT_DYNAMIC(CEnumFile, CDialogEx)

CEnumFile::CEnumFile(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ENUMFILE, pParent)
{

}

CEnumFile::~CEnumFile()
{
}

void CEnumFile::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CEnumFile, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CEnumFile::OnClickTree1)
	ON_COMMAND(ID_32778, &CEnumFile::OnDeleteFile)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CEnumFile::OnRclickList1)
END_MESSAGE_MAP()


// CEnumFile 消息处理程序


BOOL CEnumFile::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//
	WCHAR szName[MAX_PATH] = { 0 };
	GetLogicalDriveStrings(MAX_PATH, szName);
	WCHAR rootPath[10] = { 0 };
	WCHAR driveType[21] = { 0 };
	DWORD nType = 0;
	for (char ch = 'A'; ch <= 'Z'; ch++)
	{
		wsprintf(rootPath, L"%c:\\", ch);
		nType = GetDriveType(rootPath);
		if (nType == DRIVE_FIXED)//硬盘
		{
			CString buff;
			buff.Format(L"%c:", ch);
			//将所有系统盘符设置到树中
			HTREEITEM hItem = m_Tree.InsertItem(buff, NULL);
			wchar_t* pBuff = _wcsdup(buff.GetBuffer());
			m_Tree.SetItemData(hItem, (DWORD_PTR)pBuff);
		}
	}
	DWORD dwStyle = 0;
	m_ListCtrl.GetExtendedStyle();
	m_ListCtrl.SetExtendedStyle(dwStyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect cRect;
	m_ListCtrl.GetClientRect(cRect);
	m_ListCtrl.InsertColumn(0, L"文件名", 0, cRect.Width() / 4);
	m_ListCtrl.InsertColumn(1, L"文件大小", 0, cRect.Width() / 4);
	m_ListCtrl.InsertColumn(2, L"创建时间", 0, cRect.Width() / 4);
	m_ListCtrl.InsertColumn(3, L"最后修改时间", 0, cRect.Width() / 4);
	m_Menu.LoadMenuW(IDR_MENU3);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


//*****************************************************************************************
// 函数名称: OnClickTree1
// 函数说明: 点击一下树来获取到目录
// 作    者: lracker
// 时    间: 2019/12/25
// 参    数: NMHDR *
// 参    数: LRESULT *
// 返 回 值: void
//*****************************************************************************************
void CEnumFile::OnClickTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	DWORD dwChild = 0;
	CPoint pos = {};
	GetCursorPos(&pos);
	ScreenToClient(&pos);
	HTREEITEM hItem = m_Tree.HitTest(pos);
	// 判断是否有值
	if (!hItem)
		return;
	HTREEITEM hChild = m_Tree.GetNextItem(hItem, TVGN_CHILD);
	// 判断是否有子节点
	if (hChild)	
		dwChild = 1;
	CString Path = (wchar_t*)m_Tree.GetItemData(hItem);
	if (Path.IsEmpty())
		return;
	// 遍历文件了
	m_ListCtrl.DeleteAllItems();
	ENUMFILES test;
	DWORD dwSize = 0;
	// 先发出一次请求，返回所需要的大小。
	WCHAR wPath[256] = { 0 };
	memset(wPath, 0, 256);
	m_Dir = Path;
	CString LastPath;
	LastPath = L"\\??\\" + Path + L"\\";
	wcscpy_s(wPath, LastPath.GetLength() * 2, LastPath.GetBuffer());
	int len = wcslen(wPath) * 2 + 2;
	DeviceIoControl(g_hDev, ENUMFILE, wPath, len, &test, sizeof(ENUMFILES), &dwSize, NULL);
	PENUMFILES pFile = new ENUMFILES[dwSize]();
	// 发送盘符过去
	DeviceIoControl(g_hDev, ENUMFILE, wPath, len, pFile, dwSize, &dwSize, NULL);
	// 所有项数
	int nCount = dwSize / sizeof(ENUMFILES);
	int i = 0;
	int nIndex = 0;
	FILETIME CreateFt;
	FILETIME ChangeFt;
	while (nCount)
	{
		--nCount;
		// 判断是目录还是文件
		BYTE Flag = pFile[i].FileOrDirectory;
		CString Buffer = pFile[i].FileName;
		if (Flag == 0 && !dwChild)	// 目录
		{
			HTREEITEM hItem2 = m_Tree.InsertItem(Buffer, hItem);
			WCHAR* szFullPath = new WCHAR[MAX_PATH];
			StringCbPrintf(szFullPath, MAX_PATH, L"%s\\%s", Path, Buffer);  //拼接起来
			m_Tree.SetItemData(hItem2, (DWORD_PTR)szFullPath);
		}
		else if (Flag == 1) // 文件
		{
			m_ListCtrl.InsertItem(nIndex, _T(""));
			m_ListCtrl.SetItemText(nIndex, 0, Buffer);	// 文件名
			CString Temp;
			Temp.Format(L"%uB", pFile[i].Size);
			m_ListCtrl.SetItemText(nIndex, 1, Temp);	// 大小
			memcpy(&CreateFt, &pFile[i].CreateTime, sizeof(CreateFt));
			memcpy(&ChangeFt, &pFile[i].ChangeTime, sizeof(ChangeFt));
			SYSTEMTIME st = { 0 };
			FILETIME ft = { 0 };
			FileTimeToLocalFileTime(&CreateFt, &ft);
			FileTimeToSystemTime(&ft, &st);
			Temp.Format(L"%4d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			m_ListCtrl.SetItemText(nIndex, 2, Temp);	// 创建时间
			FileTimeToLocalFileTime(&ChangeFt, &ft);
			FileTimeToSystemTime(&ft, &st);
			Temp.Format(L"%4d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			m_ListCtrl.SetItemText(nIndex, 3, Temp);	// 修改时间
			++nIndex;
		}
			++i;
	}
	delete[] pFile;
	*pResult = 0;
}


//*****************************************************************************************
// 函数名称: OnDeleteFile
// 函数说明: 删除文件
// 作    者: lracker
// 时    间: 2019/12/26
// 返 回 值: void
//*****************************************************************************************
void CEnumFile::OnDeleteFile()
{
	// 拼接
	// 获取到列表名字
	CString LastPath;
	LastPath = L"\\??\\" + m_Dir + L"\\" + m_FileName;
	WCHAR wPath[256] = { 0 };
	memset(wPath, 0, 256);
	wcscpy_s(wPath, LastPath.GetLength() * 2, LastPath.GetBuffer());
	int len = wcslen(wPath) * 2 + 2;
	DWORD dwSize = 0;
	DeviceIoControl(g_hDev, DELETEFILE, wPath, len, NULL, NULL, &dwSize, NULL);
}


//*****************************************************************************************
// 函数名称: OnRclickList1
// 函数说明: 右键弹窗
// 作    者: lracker
// 时    间: 2019/12/26
// 参    数: NMHDR *
// 参    数: LRESULT *
// 返 回 值: void
//*****************************************************************************************
void CEnumFile::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CString Buffer = m_ListCtrl.GetItemText(pNMItemActivate->iItem, 0);
	m_FileName = Buffer;
	// 获取当前鼠标相对于屏幕的位置
	POINT Point = { 0 };
	GetCursorPos(&Point);
	// 获取菜单的子菜单
	CMenu* SubMenu = m_Menu.GetSubMenu(0);
	// 弹出窗口
	SubMenu->TrackPopupMenu(TPM_LEFTALIGN, Point.x, Point.y, this);
	*pResult = 0;
}
