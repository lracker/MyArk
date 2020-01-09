#pragma once


// CIDT 对话框

class CIDT : public CDialogEx
{
	DECLARE_DYNAMIC(CIDT)

public:
	CIDT(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CIDT();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IDT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CMenu m_Menu;
	CListCtrl m_ListCtrl;
	virtual BOOL OnInitDialog();
protected:
	afx_msg LRESULT OnFlushIDT(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnFlush();
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
};
