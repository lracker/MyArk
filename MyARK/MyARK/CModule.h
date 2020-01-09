#pragma once


// CModule 对话框

class CModule : public CDialogEx
{
	DECLARE_DYNAMIC(CModule)

public:
	CModule(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CModule();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MODULE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwPID;
	CListCtrl m_ListCtrl;
	VOID GetModule();
	virtual BOOL OnInitDialog();
};
