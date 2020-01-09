#pragma once


// CDriver 对话框

class CDriver : public CDialogEx
{
	DECLARE_DYNAMIC(CDriver)

public:
	CDriver(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDriver();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DRIVER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ListCtrl;
	CMenu m_Menu;
	WCHAR m_HideDriverName[256];
protected:
	afx_msg LRESULT OnFlushDriver(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFLUSHLIST();
	afx_msg void OnHideDriver();
};
