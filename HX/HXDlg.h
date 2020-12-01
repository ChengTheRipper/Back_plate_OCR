
// HXDlg.h: 头文件
//
#include "CvisionDlg.h"
#include "CmodbusDlg.h"
#include "CcadDlg.h"
#include "CLayout.h"
#pragma once


// CHXDlg 对话框
class CHXDlg : public CDialogEx
{
// 构造
public:
	CHXDlg(CWnd* pParent = nullptr);	// 标准构造函数
	//视觉窗口声明
	CvisionDlg m_Dlg1;
	//通信窗口声明
	CmodbusDlg m_Dlg2;
	//cad图纸窗口声明
	CcadDlg m_Dlg3;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HX_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tabCtrl;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	CLayout m_layout;
	
	POINT old;
	CStatusBar m_Statusbar;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CImageList m_imglist;
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CBrush m_Brush;
};
