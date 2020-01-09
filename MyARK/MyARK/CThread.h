#pragma once


// CThread 对话框

class CThread : public CDialogEx
{
	DECLARE_DYNAMIC(CThread)

public:
	CThread(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CThread();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_THREAD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwPID;
	CListCtrl m_ListCtrl;
	virtual BOOL OnInitDialog();
	VOID GetThread();
};
