#pragma once


// CSSDT 对话框

class CSSDT : public CDialogEx
{
	DECLARE_DYNAMIC(CSSDT)

public:
	CSSDT(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSSDT();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SSDT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CMenu m_Menu;
	CListCtrl m_ListCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFlushSSDT();
protected:
	afx_msg LRESULT OnFlushssdt(WPARAM wParam, LPARAM lParam);
};
