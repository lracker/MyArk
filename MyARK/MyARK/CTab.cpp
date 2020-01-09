// CTab.cpp: 实现文件
//

#include "pch.h"
#include "MyARK.h"
#include "CTab.h"


// CTab

IMPLEMENT_DYNAMIC(CTab, CTabCtrl)

CTab::CTab()
{
	m_dwTabNum = 7;
}

CTab::~CTab()
{
}


BEGIN_MESSAGE_MAP(CTab, CTabCtrl)
END_MESSAGE_MAP()



// CTab 消息处理程序


