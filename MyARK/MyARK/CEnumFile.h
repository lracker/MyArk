#pragma once


// CEnumFile 对话框

class CEnumFile : public CDialogEx
{
	DECLARE_DYNAMIC(CEnumFile)

public:
	CEnumFile(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEnumFile();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ENUMFILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_Tree;
	CListCtrl  m_ListCtrl;
	CString m_Dir;
	CString m_FileName;
	CMenu m_Menu;
	virtual BOOL OnInitDialog();
	afx_msg void OnClickTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteFile();
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
};
