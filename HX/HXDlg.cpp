
// HXDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "HX.h"
#include "HXDlg.h"
#include "afxdialogex.h"
#include "layoutinit.h"
#include "CLoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTime StatusTime;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHXDlg 对话框



CHXDlg::CHXDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HX_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_GDUT);
}

void CHXDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tabCtrl);
}

BEGIN_MESSAGE_MAP(CHXDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CHXDlg::OnTcnSelchangeTab1)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CHXDlg 消息处理程序

BOOL CHXDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//菜单栏设置
	/*CMenu menu;
	menu.LoadMenuW(IDR_MENU1);
	SetMenu(&menu);*/

	m_imglist.Create(32, 32, ILC_COLOR32 | ILC_MASK, 2, 1);
	m_imglist.Add(AfxGetApp()->LoadIconW(IDI_VISION));
	m_imglist.Add(AfxGetApp()->LoadIconW(IDI_CAD));
	m_imglist.Add(AfxGetApp()->LoadIconW(IDI_MODBUS));
	m_tabCtrl.SetImageList(&m_imglist);

	m_tabCtrl.InsertItem(0, _T("视觉处理"),0);
	m_tabCtrl.InsertItem(1, _T("CAD图纸"),1);
	m_tabCtrl.InsertItem(2, _T("串口通信"),2);
	// 创建三个子对话框
	m_Dlg1.Create(IDD_VISION, &m_tabCtrl);
	m_Dlg3.Create(IDD_CAD, &m_tabCtrl);
	m_Dlg2.Create(IDD_MODBUS, &m_tabCtrl);
	// 调整子对话框大小及位置
	CRect rc;
	m_tabCtrl.GetClientRect(&rc);
	CRect rcTabItem;
	m_tabCtrl.GetItemRect(0, rcTabItem);
	rc.top += rcTabItem.Height() + 4;
	rc.left += 4;
	rc.bottom -= 4;
	rc.right -= 4;
	m_Dlg1.MoveWindow(&rc);
	m_Dlg2.MoveWindow(&rc);
	m_Dlg3.MoveWindow(&rc);
	// 默认标签选中
	m_Dlg1.ShowWindow(SW_SHOW);
	m_tabCtrl.SetCurFocus(0);


	CSize sizeTabHeader;
	sizeTabHeader.cx = 0;
	sizeTabHeader.cy = 30;
	m_tabCtrl.SetItemSize(sizeTabHeader);

	CRect rect;
	GetClientRect(&rect);
	old.x = rect.right - rect.left;
	old.y = rect.bottom - rect.top;


	//给状态栏添加时间更新
	SetTimer(1, 1000, NULL);

	//添加状态栏
	//获得系统当前时间
	//CTime StatusTime;
	StatusTime = StatusTime.GetCurrentTime();
	CString InitTime = StatusTime.Format("%Y-%m-%d %H:%M:%S");


	//创建状态栏
	UINT array[2] = { 12301,12302 };    //注：这里是ID号，应保证不与resource.h中的其他ID号重复
	m_Statusbar.Create(this);
	m_Statusbar.SetIndicators(array, sizeof(array) / sizeof(UINT));

	//显示状态栏
	CRect StatusRect;
	GetWindowRect(StatusRect);
	m_Statusbar.SetPaneInfo(0, array[0], 0, StatusRect.Width() / 3);
	m_Statusbar.SetPaneInfo(1, array[1], 0, StatusRect.Width() / 3 * 2);
	m_Statusbar.SetPaneText(0, InitTime);
	m_Statusbar.SetPaneText(1, _T("GDUT"));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);


	
	
	//这个也是无法准确获得分辨率
	/*int cx, cy;
	cx = GetSystemMetrics(SM_CXSCREEN);
	cy = GetSystemMetrics(SM_CYSCREEN);

	CRect rcTemp;
	rcTemp.BottomRight() = CPoint(cx, cy);
	rcTemp.TopLeft() = CPoint(0, 0);
	
	MoveWindow(&rcTemp);*/

	//全屏操作 换肤之后彻底不行
	//ModifyStyle(WS_CAPTION, 0, 0);  // 如果只是要普通的全屏，不想去掉标题栏，就不用第一个语句
	//SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	//全屏幕操作
	WINDOWPLACEMENT m_struOldWndpl;
	//get current system resolution
	int g_iCurScreenWidth = GetSystemMetrics(SM_CXSCREEN); //1920
	int g_iCurScreenHeight = GetSystemMetrics(SM_CYSCREEN); //1080

	//for full screen while backplay
	GetWindowPlacement(&m_struOldWndpl);

	CRect rectWholeDlg;//entire client(including title bar)
	CRect rectClient;//client area(not including title bar)
	CRect rectFullScreen;
	//用于接收左上角和右下角的屏幕坐标
	GetWindowRect(&rectWholeDlg);
	RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposQuery, &rectClient);
	//将显示器上给定点或矩形的客户去坐标转换为屏幕坐标 新的坐标是相对于屏幕左上角的
	ClientToScreen(&rectClient);

	//-8 = 0 - 8
	rectFullScreen.left = rectWholeDlg.left - rectClient.left;
	//-28 = 0 - 28
	rectFullScreen.top = rectWholeDlg.top - rectClient.top - 15;
	// = 1088 + 1920 - 1080
	rectFullScreen.right = rectWholeDlg.right + g_iCurScreenWidth - rectClient.right;
	// = 639 + 1080 - 609
	rectFullScreen.bottom = rectWholeDlg.bottom + g_iCurScreenHeight - rectClient.bottom - 10;
	
	/*rectFullScreen.right = g_iCurScreenWidth;
	rectFullScreen.bottom = g_iCurScreenHeight;*/
	//CString tt;
	//tt.Format(_T("%d"), rectFullScreen.right);//前后之差即程序运行时间  
	//MessageBox(tt);

	//enter into full screen;
	WINDOWPLACEMENT struWndpl;
	struWndpl.length = sizeof(WINDOWPLACEMENT);
	struWndpl.flags = 0;
	struWndpl.showCmd = SW_SHOWNORMAL;
	struWndpl.rcNormalPosition = rectFullScreen;
	SetWindowPlacement(&struWndpl);

	//全屏操作
	/*CRect rectDeskTop;
	WINDOWPLACEMENT wpnew;
	GetWindowPlacement(&wpnew);
	::GetWindowRect(::GetDesktopWindow(), rectDeskTop);
	::AdjustWindowRectEx(rectDeskTop, GetStyle(), FALSE, GetExStyle()); 这个自适应出问题
	wpnew.showCmd = SW_SHOWNORMAL;
	wpnew.rcNormalPosition = rectDeskTop;
	SetWindowPlacement(&wpnew);*/


	//程序自适应全屏
	//这个不行
	//背景颜色画刷
	m_Brush.CreateSolidBrush(RGB(240, 240, 220));

	InitLayout(m_layout, this);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHXDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHXDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CHXDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CHXDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//声明密码登录窗口
	
	switch (m_tabCtrl.GetCurSel())
	{
		//视觉窗口
	case 0:
		m_Dlg1.ShowWindow(SW_SHOW);
		m_Dlg2.ShowWindow(SW_HIDE);
		m_Dlg3.ShowWindow(SW_HIDE);
		m_Dlg1.SetFocus();
		break;
		//cad窗口
	case 1:
		//dlg.DoModal();
		if (LoginFlag == true)
		{
			m_Dlg3.ShowWindow(SW_SHOW);
			m_Dlg2.ShowWindow(SW_HIDE);
			m_Dlg1.ShowWindow(SW_HIDE);
			m_Dlg3.SetFocus();
			break;
		}
		else
		{
			m_Dlg3.ShowWindow(SW_HIDE);
			m_Dlg2.ShowWindow(SW_HIDE);
			m_Dlg1.ShowWindow(SW_HIDE);
			m_Dlg3.SetFocus();
			break;
		}
		
		//图纸窗口
	case 2:
		//dlg.DoModal();
		if (LoginFlag == true)
		{
			m_Dlg2.ShowWindow(SW_SHOW);
			m_Dlg1.ShowWindow(SW_HIDE);
			m_Dlg3.ShowWindow(SW_HIDE);
			m_Dlg2.SetFocus();
			break;
		}
		else
		{
			m_Dlg2.ShowWindow(SW_HIDE);
			m_Dlg1.ShowWindow(SW_HIDE);
			m_Dlg3.ShowWindow(SW_HIDE);
			m_Dlg2.SetFocus();
			break;
		}
	default:
		break;
	}

	*pResult = 0;
}


void CHXDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (1)
	{
		float fsp[2];
		POINT Newp; //获取现在对话框的大小
		CRect recta;
		GetClientRect(&recta);     //取客户区大小  
		Newp.x = recta.right - recta.left;
		Newp.y = recta.bottom - recta.top;
		fsp[0] = (float)Newp.x / old.x;
		fsp[1] = (float)Newp.y / old.y;
		CRect Rect;
		int woc;
		CPoint OldTLPoint, TLPoint; //左上角
		CPoint OldBRPoint, BRPoint; //右下角
		HWND  hwndChild = ::GetWindow(m_hWnd, GW_CHILD);  //列出所有控件  
		while (hwndChild)
		{
			woc = ::GetDlgCtrlID(hwndChild);//取得ID
			GetDlgItem(woc)->GetWindowRect(Rect);
			ScreenToClient(Rect);
			OldTLPoint = Rect.TopLeft();
			TLPoint.x = long(OldTLPoint.x*fsp[0]);
			TLPoint.y = long(OldTLPoint.y*fsp[1]);
			OldBRPoint = Rect.BottomRight();
			BRPoint.x = long(OldBRPoint.x *fsp[0]);
			BRPoint.y = long(OldBRPoint.y *fsp[1]);
			Rect.SetRect(TLPoint, BRPoint);
			GetDlgItem(woc)->MoveWindow(Rect, TRUE);
			hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
		}
		old = Newp;
	}

	// TODO: 在此处添加消息处理程序代码
	m_layout.OnSize(cx, cy);

	if (m_Dlg1.GetSafeHwnd() || m_Dlg2.GetSafeHwnd() || m_Dlg3.GetSafeHwnd())
	{
		CRect rctab;
		CRect reItem;
		int m_nWidth, m_nHeight;

		m_tabCtrl.GetClientRect(&rctab);

		m_nWidth = rctab.left - rctab.right;
		m_nHeight = rctab.bottom - rctab.top;

		m_tabCtrl.GetItemRect(0, &reItem);
		rctab.DeflateRect(0, reItem.bottom, 0, 0);

		m_Dlg1.MoveWindow(&rctab);
		m_Dlg2.MoveWindow(&rctab);
		m_Dlg3.MoveWindow(&rctab);
	}
}


void CHXDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
}


void CHXDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 1:
		StatusTime = StatusTime.GetCurrentTime();
		CString sTime = StatusTime.Format("%Y-%m-%d %H:%M:%S");
		m_Statusbar.SetPaneText(0, sTime);
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CHXDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此处添加实现代码.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)    //屏蔽回车和ESC  
			return TRUE;
		if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4)  //屏蔽ALT+F4
			return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


HBRUSH CHXDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	//switch (nCtlColor)
	//{
	//	case CTLCOLOR_STATIC:
	//	{
	//		pDC->SetTextColor(RGB(50, 50, 200));  //字体颜色
	//		pDC->SetBkColor(RGB(240, 240, 220));   //字体背景色
	//		pDC->SetBkMode(TRANSPARENT); //设置字体背景为透明
	//		return (HBRUSH)m_Brush.GetSafeHandle();
	//	}
	//}
	//return (HBRUSH)m_Brush.GetSafeHandle();;
	return hbr;
}
