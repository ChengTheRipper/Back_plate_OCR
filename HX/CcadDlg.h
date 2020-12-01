#pragma once

//#include "CSerialPort/SerialPort.h"
//#include "CSerialPort/SerialPortInfo.h"
//#include "CLayout.h"
#include "CmodbusDlg.h"
#include "CLayout.h"
//using namespace itas109;

// CcadDlg 对话框

class CcadDlg : public CDialogEx, public has_slots<>
{
	DECLARE_DYNAMIC(CcadDlg)

public:
	CcadDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CcadDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	virtual BOOL OnInitDialog();
	//CSerialPort m_Cad_SerialPort;//About CSerialPort 
	unsigned short CRC16(unsigned char* puchMsg, unsigned short usDataLen);
	void SendData(int CommTypeIn, WORD DownAdd, DWORD DownData);
	
	afx_msg void OnBnClickedButtonCadOpen();
	afx_msg void OnBnClickedButtonCadDraw();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CLayout m_layoutCad;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	CListCtrl m_CadGlueList;
	
	afx_msg void OnBnClickedButtonCadSend();
	BOOL PreTranslateMessage(MSG* pMsg);
};
