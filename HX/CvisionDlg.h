#pragma once
#include "CmodbusDlg.h"
#include "CLayout.h"


// CvisionDlg 对话框

class CvisionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CvisionDlg)

public:
	CvisionDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CvisionDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VISION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	unsigned short CRC16(unsigned char* puchMsg, unsigned short usDataLen);
	void SendData(int CommTypeIn, WORD DownAdd, DWORD DownData);
	afx_msg void OnBnClickedVisionButtonLog();
	afx_msg void OnBnClickedVisionButtonExit();
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CLayout m_layoutVision;
	virtual BOOL OnInitDialog();
};
