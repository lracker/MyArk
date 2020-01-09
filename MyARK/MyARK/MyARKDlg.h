
// MyARKDlg.h: 头文件
//

#pragma once
#include "Data.h"
#include "CTab.h"
#include <winsvc.h>

// CMyARKDlg 对话框
class CMyARKDlg : public CDialogEx
{
// 构造
public:
	CMyARKDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYARK_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwLastError;
	CString m_Path;
	SC_HANDLE m_hSCManager;
	SC_HANDLE m_hService;
	CTab m_TabCtrl;
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	// 加载驱动
	VOID LoadDriver();
	// 启动驱动
	VOID StartDriver();
	// 停止驱动
	VOID StopDriver();
	// 卸载驱动
	VOID UnLoadDriver();
	virtual BOOL DestroyWindow();
};
