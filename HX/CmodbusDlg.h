#pragma once
//About CSerialPort start
#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
#include "CLayout.h"

using namespace itas109;



extern int BadNum;
extern int SendGlueNum;
extern int SendLocationNum;
extern bool RecMsgFlag;
extern bool OverTime;
extern long m_CadT1;
extern long m_CadT2;
extern bool SendOnce;

//判断背板是否到达
extern bool ArriveFlag;
//是否执行
extern bool ExecuteIdentify;
//判断是否识别完成
extern bool IdentifyDone;

// CmodbusDlg 对话框

class CmodbusDlg : public CDialogEx , public has_slots<>//About CSerialPort
{
	DECLARE_DYNAMIC(CmodbusDlg)

public:
	CmodbusDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CmodbusDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MODBUS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
private:
	 
	CString m_EditSend;
	CComboBox m_comb1;
	CComboBox m_comb2;
	CComboBox m_comb3;
	CComboBox m_comb4;
	CComboBox m_comb5;

private:
	CString m_EditReceive;
public:
	afx_msg void OnBnClickedButtonOpen();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSendOnce();
	unsigned short CRC16(unsigned char* puchMsg, unsigned short usDataLen);
	char HexChar(char c);
	void StringtoHex(BYTE * GB, int glen, BYTE* SB, int* slen);
	void OnReceive();
	CButton m_OpenCloseCtrl;
	void SendData(int CommTypeIn, WORD DownAdd, DWORD DownData);
	afx_msg void OnBnClickedButton1();
	void ReadArrive();
	void SendGlueData();
	void SendLocationData();
private:
	/*CListCtrl m_GlueList;
	CListCtrl m_IdentifyList;*/
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CEdit m_EditReceiveCtrl;
	CLayout m_layoutMod;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	
	
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonClean();

	CSerialPort m_SerialPort;//About CSerialPort
	static CmodbusDlg *pModbusdlg;//给别的窗口调用本窗口里的变量用的
	afx_msg void OnPaint();
	CBrush m_Brush;
	BOOL PreTranslateMessage(MSG* pMsg);
};
