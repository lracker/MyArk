// CRegister.cpp: 实现文件
//

#include "pch.h"
#include "MyARK.h"
#include "CRegister.h"
#include "afxdialogex.h"
#include "Data.h"
#include <strsafe.h>


// CRegister 对话框

IMPLEMENT_DYNAMIC(CRegister, CDialogEx)

CRegister::CRegister(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REGISTER, pParent)
{

}

CRegister::~CRegister()
{
}

void CRegister::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
}


BEGIN_MESSAGE_MAP(CRegister, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CRegister::OnClickTree1)
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, &CRegister::OnRclickTree1)
	ON_COMMAND(ID_32782, &CRegister::OnNew)
	ON_COMMAND(ID_32783, &CRegister::OnDelete)
END_MESSAGE_MAP()


// CRegister 消息处理程序


BOOL CRegister::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Menu.LoadMenuW(IDR_MENU7);

	DWORD dwStyle = 0;
	m_ListCtrl.GetExtendedStyle();
	m_ListCtrl.SetExtendedStyle(dwStyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect cRect;
	m_ListCtrl.GetClientRect(cRect);
	m_ListCtrl.InsertColumn(0, L"名称", 0, cRect.Width() / 3);
	m_ListCtrl.InsertColumn(1, L"类型", 0, cRect.Width() / 3);
	m_ListCtrl.InsertColumn(2, L"数据", 0, cRect.Width() / 3);
	WCHAR Buffer[256] = {};
	memset(Buffer, 0, 256);
	CString Root = L"\\Registry";
	memcpy(Buffer, Root, Root.GetLength() * 2);
	DWORD dwSize = 0;
	DeviceIoControl(g_hDev, ENUMREGISTER, Buffer, wcslen(Root.GetBuffer()) * 2 + 2, NULL, NULL, &dwSize, NULL);
	int nCount = dwSize / sizeof(REGISTER);
	PREGISTER pReg = new REGISTER[nCount]();
	DeviceIoControl(g_hDev, ENUMREGISTER, Buffer, wcslen(Root.GetBuffer()) * 2 + 2, pReg, dwSize, &dwSize, NULL);
	for (int i = 0; i < nCount; ++i)
	{
		// 根据TYPE来给空间分配
		if (pReg[i].Type == 0) // 子项
		{
			CString buff = pReg[i].KeyName;
			CString Path;
			if (buff == L"MACHINE")
			{ 
				Path = L"\\Registry\\Machine";
				buff = L"HKEY_LOCAL_MACHINE";
			}
			else if (buff == L"USER")
			{
				Path = L"\\Registry\\user";
				buff = L"HKEY_USERS";
			}
			else if (buff == L"A")
			{
				Path = L"\\Registry\\user"; // 有问题
				buff = L"A";
			}
			//将所有系统盘符设置到树中
			HTREEITEM hItem = m_Tree.InsertItem(buff, NULL);
			wchar_t* pBuff = _wcsdup(Path.GetBuffer());
			m_Tree.SetItemData(hItem, (DWORD_PTR)pBuff);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CRegister::OnClickTree1(NMHDR* pNMHDR, LRESULT* pResult)
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
	m_ListCtrl.DeleteAllItems();

	REGISTER test;
	DWORD dwSize = 0;
	// 先发出一次请求，返回所需要的大小。
	WCHAR wPath[256] = { 0 };
	memset(wPath, 0, 256);
	CString LastPath;
	LastPath = Path;
	wcscpy_s(wPath, LastPath.GetLength() * 2, LastPath.GetBuffer());
	int len = wcslen(wPath) * 2 + 2;
	DeviceIoControl(g_hDev, ENUMREGISTER, wPath, len, &test, sizeof(REGISTER), &dwSize, NULL);
	PREGISTER pReg = new REGISTER[dwSize]();
	// 发送盘符过去
	DeviceIoControl(g_hDev, ENUMREGISTER, wPath, len, pReg, dwSize, &dwSize, NULL);
	int nNum = dwSize / sizeof(REGISTER);
	int nIndex = 0;
	for (int i = 0; i < nNum; ++i)
	{
		// 判断类型
		if (pReg[i].Type == 0 && !dwChild) // 子项
		{
			CString buff = pReg[i].KeyName;
			HTREEITEM hItem2 = m_Tree.InsertItem(buff, hItem);
			WCHAR* szFullPath = new WCHAR[MAX_PATH];
			StringCbPrintf(szFullPath, MAX_PATH, L"%s\\%s", Path, buff);  //拼接起来
			m_Tree.SetItemData(hItem2, (DWORD_PTR)szFullPath);
		}
		else if (pReg[i].Type == 1) // 键
		{
			CString buff2;
			CString buff = pReg[i].ValueName;
			m_ListCtrl.InsertItem(nIndex, _T(""));
			m_ListCtrl.SetItemText(nIndex, 0, buff);	// 文件名
			if (pReg[i].ValueType == REG_SZ)
			{
				buff = L"REG_SZ";
				buff2.Format(L"%s", pReg[i].Value);
			}
			else if (pReg[i].ValueType == REG_MULTI_SZ)
			{
				buff = L"REG_MULTI_SZ";
				buff2.Format(L"%s", pReg[i].Value);
			}
			else if (pReg[i].ValueType == REG_DWORD)
			{
				buff = L"REG_DWORD";
				buff2.Format(L"0x%08x", pReg[i].Value);
			}
			else if (pReg[i].ValueType == REG_BINARY)
			{
				buff = L"REG_BINARY";
				int nSize = strlen((char*)pReg[i].Value);
				buff2 = L"";
				CString temp;
				for (int j = 0; j < nSize; ++j)
				{
					buff2 += L" ";
					temp.Format(L"%02X", (unsigned char)pReg[i].Value[j]);
					buff2 += temp;
				}
			}
			m_ListCtrl.SetItemText(nIndex, 1, buff);	// 文件名
			
			m_ListCtrl.SetItemText(nIndex, 2, buff2);	// 数据
			nIndex++;
		}
	}
	*pResult = 0;
}



void CRegister::OnRclickTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint pos = {};
	GetCursorPos(&pos);
	ScreenToClient(&pos);
	HTREEITEM hItem = m_Tree.HitTest(pos);
	// 判断是否有值
	if (!hItem)
		return;
	CString Path = (wchar_t*)m_Tree.GetItemData(hItem);
	if (Path.IsEmpty())
		return;
	m_FileName = Path;
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
// 函数名称: OnNew
// 函数说明: 创建子项
// 作    者: lracker
// 时    间: 2019/12/28
// 返 回 值: void
//*****************************************************************************************
void CRegister::OnNew()
{
	WCHAR wPath[256] = { 0 };
	CString LastPath;
	LastPath = m_FileName + L"\\15PB";
	wcscpy_s(wPath, LastPath.GetLength() * 2, LastPath.GetBuffer());
	int len = wcslen(wPath) * 2 + 2;
	DWORD dwSize = 0;
	DeviceIoControl(g_hDev, NEWREG, wPath, len, NULL, NULL, &dwSize, NULL);
}


//*****************************************************************************************
// 函数名称: OnDelete
// 函数说明: 删除子项
// 作    者: lracker
// 时    间: 2019/12/28
// 返 回 值: void
//*****************************************************************************************
void CRegister::OnDelete()
{
	WCHAR wPath[256] = { 0 };
	CString LastPath;
	LastPath = m_FileName;
	wcscpy_s(wPath, LastPath.GetLength() * 2, LastPath.GetBuffer());
	int len = wcslen(wPath) * 2 + 2;
	DWORD dwSize = 0;
	DeviceIoControl(g_hDev, DELETEREG, wPath, len, NULL, NULL, &dwSize, NULL);
}
