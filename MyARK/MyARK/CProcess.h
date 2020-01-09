#pragma once


// CProcess 对话框

class CProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CProcess)

public:
	CProcess(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CProcess();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwPID;
	CMenu m_Menu;
	CListCtrl m_ListCtrl;
	virtual BOOL OnInitDialog();
protected:
	afx_msg LRESULT OnFlushProcess(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFlushList();
	afx_msg void OnThread();
	afx_msg void OnModule();
	afx_msg void OnHideProcess();
	afx_msg void OnTerminateProcess();
};
