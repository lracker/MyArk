#pragma once


// CTab

class CTab : public CTabCtrl
{
	DECLARE_DYNAMIC(CTab)

public:
	CTab();
	virtual ~CTab();
	DWORD m_dwTabNum;
	CDialogEx* m_Dia[7];
protected:
	DECLARE_MESSAGE_MAP()
};


