#pragma once


// CGDT 对话框

class CGDT : public CDialogEx
{
	DECLARE_DYNAMIC(CGDT)

public:
	CGDT(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGDT();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GDT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CMenu m_Menu;
	CListCtrl m_ListCtrl;
	virtual BOOL OnInitDialog(); 
	afx_msg void OnFlush();
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnFlushgdt(WPARAM wParam, LPARAM lParam);	
};
