// CmodbusDlg.cpp: 实现文件
//

#include "pch.h"
#include "HX.h"
#include "CmodbusDlg.h"
#include "afxdialogex.h"
#include <vector>
#include "layoutinitModbus.h"



int BaudRateArray[] = { 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 56000, 57600, 115200 };

std::string ParityArray[] = { "None", "Odd", "Even", "Mark", "Space" };

std::string DataBitsArray[] = { "5", "6", "7","8" };

std::string StopArray[] = { "1", "1.5", "2" };

CmodbusDlg *CmodbusDlg::pModbusdlg = NULL;

//判断从机发送回复信息的对错 可以进入第一次发送
bool RecMsgFlag = true;
//接收超时 超时好像设置不了
bool OverTime = false;
//发送的胶条数量
int SendGlueNum = 0;
//发送的定位数据数量
int SendLocationNum = 0;
int BadNum = 0;
//用以测试接收到多少次正确数据
int RecNum = 0;
//T1为开关按下时刻，T2为数据接收时刻
long m_CadT1;
//给T2赋一个初值，防止首次执行时发生误判，之后的时候若T2值为0则说明通信断线，我们没有收到接收函数
long m_CadT2 = 1;
//判断背板是否到达
bool ArriveFlag = false;
//是否执行
bool ExecuteIdentify = true;
//判断是否识别完成
bool IdentifyDone = false;
//胶条数据的总数量，是从sendGlueData赋值
//int vecGlueNum;
//发送胶条时计数
//int locGlueNum = 0;

//声明一个当前是否是单次发送的flag
bool SendOnce = false;



//这个数据暂时不用发完就清空，因为每一次都会把对应的值覆盖进去
//WORD GlueTemp[200];//把胶条数据从函数里边提取出来变成全局的，用以发送
// CmodbusDlg 对话框

IMPLEMENT_DYNAMIC(CmodbusDlg, CDialogEx)

CmodbusDlg::CmodbusDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MODBUS, pParent)
	, m_EditSend(_T(""))
	, m_EditReceive(_T(""))
{

}

CmodbusDlg::~CmodbusDlg()
{
}

void CmodbusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_EditSend);
	DDX_Control(pDX, IDC_COMBO1, m_comb1);
	DDX_Control(pDX, IDC_COMBO2, m_comb2);
	DDX_Control(pDX, IDC_COMBO3, m_comb3);
	DDX_Control(pDX, IDC_COMBO4, m_comb4);
	DDX_Control(pDX, IDC_COMBO5, m_comb5);

	DDX_Text(pDX, IDC_EDIT2, m_EditReceive);
	DDX_Control(pDX, IDC_BUTTON_OPEN, m_OpenCloseCtrl);
	
	
	DDX_Control(pDX, IDC_EDIT2, m_EditReceiveCtrl);
	
}


BEGIN_MESSAGE_MAP(CmodbusDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CmodbusDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SEND_ONCE, &CmodbusDlg::OnBnClickedButtonSendOnce)
	ON_BN_CLICKED(IDC_BUTTON1, &CmodbusDlg::OnBnClickedButton1)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_CLEAN, &CmodbusDlg::OnBnClickedButtonClean)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CmodbusDlg 消息处理程序
//窗口初始化
BOOL CmodbusDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	pModbusdlg = this;
	// 串口选择组合框
	CString temp;
	//添加波特率到下拉列表
	for (int i = 0; i < sizeof(BaudRateArray) / sizeof(int); i++)
	{
		temp.Format(_T("%d"), BaudRateArray[i]);
		m_comb2.InsertString(i, temp);
	}

	temp.Format(_T("%d"), 19200);
	m_comb2.SetCurSel(m_comb2.FindString(0, temp));

	//校验位
	for (int i = 0; i < sizeof(ParityArray) / sizeof(std::string); i++)
	{
#ifdef UNICODE
		temp.Format(_T("%S"), ParityArray[i].c_str());
#else
		temp.Format(_T("%s"), ParityArray[i].c_str());
#endif
		m_comb3.InsertString(i, temp);
	}
	m_comb3.SetCurSel(2);

	//数据位
	for (int i = 0; i < sizeof(DataBitsArray) / sizeof(std::string); i++)
	{
#ifdef UNICODE
		temp.Format(_T("%S"), DataBitsArray[i].c_str());
#else
		temp.Format(_T("%s"), DataBitsArray[i].c_str());
#endif
		m_comb4.InsertString(i, temp);
	}
	m_comb4.SetCurSel(3);

	//停止位
	for (int i = 0; i < sizeof(StopArray) / sizeof(std::string); i++)
	{
#ifdef UNICODE
		temp.Format(_T("%S"), StopArray[i].c_str());
#else
		temp.Format(_T("%s"), StopArray[i].c_str());
#endif
		m_comb5.InsertString(i, temp);
	}
	m_comb5.SetCurSel(0);

	//获取串口号  这个是获取当前可用的串口
	std::vector<SerialPortInfo> m_portsList = CSerialPortInfo::availablePortInfos();
	TCHAR m_regKeyValue[255];
	for (int i = 0; i < m_portsList.size(); i++)
	{
#ifdef UNICODE
		int iLength;
		const char * _char = m_portsList[i].portName.c_str();
		iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
		MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, m_regKeyValue, iLength);
#else
		strcpy_s(m_regKeyValue, 255, m_portsList[i].portName.c_str());
#endif
		m_comb1.AddString(m_regKeyValue);
	}
	m_comb1.SetCurSel(0);

	//OnBnClickedButtonOpenClose();

	////胶条列表
	//CString strglue[] = { _T("条数"),_T("X1"),_T("Y1") ,_T("X2"),_T("Y2"),_T("是否喷胶") };
	//for (int i = 0; i < 6; i++)
	//{
	//	//设置表头 索引 内容 对齐方式 列宽度
	//	m_GlueList.InsertColumn(i, strglue[i], LVCFMT_LEFT, 100);
	//}
	//m_GlueList.GetScrollPos(SB_VERT); //设置垂直滚动条
	//DWORD dwStyle1 = m_GlueList.GetExtendedStyle();
	//dwStyle1 |= LVS_EX_GRIDLINES;
	//m_GlueList.SetExtendedStyle(dwStyle1);//设置扩展风格
	//m_GlueList.DeleteAllItems(); 清空所有表项
	//表头不算正文里的内容，索引从0开始
	//GlueList.InsertItem(0, _T("1"));
	//给这个Item插入其他列的数据
	//GlueList.SetItemText(0,1,_T("男"))第0行第1列

	//定位信息表
	//CString stridentify[] = { _T("坐标"),_T("X坐标"),_T("Y坐标"),_T("Theta") };
	//for (int i = 0; i < 4; i++)
	//{
	//	m_IdentifyList.InsertColumn(i, stridentify[i], LVCFMT_LEFT, 100);
	//}
	//m_IdentifyList.GetScrollPos(SB_VERT); //设置垂直滚动条
	//DWORD dwStyle2 = m_IdentifyList.GetExtendedStyle();
	//dwStyle2 |= LVS_EX_GRIDLINES;
	//m_IdentifyList.SetExtendedStyle(dwStyle2);//设置扩展风格

	m_SerialPort.readReady.connect(this, &CmodbusDlg::OnReceive);

	m_Brush.CreateSolidBrush(RGB(240, 240, 220));

	InitLayoutModbus(m_layoutMod, this);

	//CStatic* pWnd = (CStatic*)GetDlgItem(IDC_PIC); // 得到 Picture Control 句柄
	//pWnd->ModifyStyle(0, SS_BITMAP); // 修改它的属性为位图
	//pWnd->SetBitmap((HBITMAP)::LoadImage(NULL, _T("1.bmp"),
	//	IMAGE_BITMAP,
	//	100,
	//	50,
	//	LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
//设置背景颜色
HBRUSH CmodbusDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC:
		{
		pDC->SetTextColor(RGB(50, 50, 200));  //字体颜色
		pDC->SetBkColor(RGB(240, 240, 220));   //字体背景色
		pDC->SetBkMode(TRANSPARENT); //设置字体背景为透明
		return (HBRUSH)m_Brush.GetSafeHandle();
		//return (HBRUSH)::GetStockObject(NULL_BRUSH);
		}
	
	}
	//if (nCtlColor == CTLCOLOR_STATIC)//如果当前控件属于静态文本
	//{
	//	//pDC->SetTextColor(RGB(255, 0, 0)); //字体颜色
	//	pDC->SetTextColor(RGB(50, 50, 200));  //字体颜色
	//	pDC->SetBkColor(RGB(240, 240, 220));   //字体背景色
	//	//pDC->SetBkMode(TRANSPARENT); //设置字体背景为透明
	//	//pDC-> SetBkColor(RGB(0, 0, 255));  //字体背景色
	//}
	//if (nCtlColor == CTLCOLOR_BTN) //如果当前控件属于按钮
	//{
	//	pDC->SetTextColor(RGB(50, 50, 200)); //字体颜色
	//	pDC->SetBkMode(TRANSPARENT); //设置字体背景为透明
	//	//pDC-> SetBkColor(RGB(0, 0, 255));  //字体背景色
	//}
	//if (pWnd->GetDlgCtrlID() == IDC_STATIC10)
	//{
	//	pDC->SetTextColor(RGB(50, 50, 200));  //字体颜色
	//	pDC->SetBkColor(RGB(240, 240, 220));   //字体背景色
 //// TODO: Return a different brush if the default is not desired
	//	return (HBRUSH)m_Brush.GetSafeHandle();  // 设置背景色
	//}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
	//return (HBRUSH)m_Brush.GetSafeHandle();
	//return (HBRUSH)::GetStockObject(WHITE_BRUSH);
}
//打开串口
void CmodbusDlg::OnBnClickedButtonOpen()
{
	CString temp;
	m_OpenCloseCtrl.GetWindowText(temp);///获取按钮的文本
	UpdateData(true);
	if (temp == _T("关闭串口"))///表示点击后是"关闭串口"，也就是已经关闭了串口
	{
		m_SerialPort.close();
		m_OpenCloseCtrl.SetWindowText(_T("打开串口"));///设置按钮文字为"打开串口"
	}
	///打开串口操作
	else if (m_comb1.GetCount() > 0)///当前列表的内容个数
	{
		string portName;
		int SelBaudRate;
		int SelParity;
		int SelDataBits;
		int SelStop;

		UpdateData(true);
		m_comb1.GetWindowText(temp);///CString temp
#ifdef UNICODE
		portName = CW2A(temp.GetString());
#else
		portName = temp.GetBuffer();
#endif	
		//CPublic::Port = portName;
		

		m_comb2.GetWindowText(temp);
		SelBaudRate = _tstoi(temp);
		//CPublic::BaudRate = SelBaudRate;

		SelParity = m_comb3.GetCurSel();
		//CPublic::Parity = SelParity;

		m_comb4.GetWindowText(temp);
		SelDataBits = _tstoi(temp);
		//CPublic::DataBits = SelDataBits;

		SelStop = m_comb5.GetCurSel();
		//CPublic::Stop = SelStop;


		m_SerialPort.init(portName, SelBaudRate, itas109::Parity(SelParity), itas109::DataBits(SelDataBits), itas109::StopBits(SelStop));
		m_SerialPort.open();
		m_SerialPort.setMinByteReadNotify(1);

		if (m_SerialPort.isOpened())
		{
			m_OpenCloseCtrl.SetWindowText(_T("关闭串口"));
		}
		else
		{
			AfxMessageBox(_T("串口已被占用！"));
		}
	}
	else
	{
		AfxMessageBox(_T("没有发现串口！"));
	}
}
//串口单次发送
void CmodbusDlg::OnBnClickedButtonSendOnce()
{
	// TODO: 在此添加控件通知处理程序代码

	SendOnce = true;

	CByteArray HexDataBuf;
	char sendData[8] = { 0 };
	int i = 0;
	BYTE SendBuf[128] = { 0 };
	BYTE GetData[256] = { 0 };
	int SendLen = 0;
	int GetLen = 0;
	unsigned short CRCData;

	UpdateData(TRUE);
	if (m_EditSend.IsEmpty())
	{
		AfxMessageBox(_T("发送数据为空!"));
		return;
	}
	HexDataBuf.RemoveAll(); //清空数组
	GetLen = m_EditSend.GetLength();
	for (i = 0; i < GetLen; i++)
	{
		GetData[i] = (BYTE)m_EditSend.GetBuffer()[i];
	}
	StringtoHex(GetData, GetLen, SendBuf, &SendLen); //将字符串转化为字节数据

	CRCData = CRC16(SendBuf, SendLen);
	SendBuf[6] = CRCData >> 8;
	SendBuf[7] = CRCData - 256 * SendBuf[6];

	HexDataBuf.SetSize(8); //设置数组大小为帧长度
	for (i = 0; i < 8; i++)
	{
		HexDataBuf.SetAt(i, SendBuf[i]);
	}
	for (int i = 0; i < 8; i++)
	{
		sendData[i] = HexDataBuf[i];
	}
	

	//int len1 = HexDataBuf.GetSize();
	//CString temp('x', len1);
	//for (int i = 0; i < len1; i++)
	//{
	//	temp.SetAt(i, HexDataBuf.GetAt(i));
	//}


	//int n = temp.GetLength();//n= 8
	//int len = WideCharToMultiByte(CP_ACP, 0, temp, temp.GetLength(), NULL, 0, NULL, NULL);//len = 8
	//char *m_str = new char[len + 1];
	//WideCharToMultiByte(CP_ACP, 0, temp, temp.GetLength(), m_str, len, NULL, NULL);
	//m_str[len + 1] = '\0';

	m_SerialPort.writeData(sendData, sizeof(sendData));

	//发送一次清空编辑框
	m_EditSend = _T("");	//给接收编辑框发送空格符
	UpdateData(false);		//更新数据

}
//16位CRC校验
unsigned short CmodbusDlg::CRC16(unsigned char* puchMsg, unsigned short usDataLen)
{
	// TODO: 在此处添加实现代码.
	static unsigned char auchCRCHi[] =
	{
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
		0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
		0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
		0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
		0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
	};

	static unsigned char auchCRCLo[] =
	{
		0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
		0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
		0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
		0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
		0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
		0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
		0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
		0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
		0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
		0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
		0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
		0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
		0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
		0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
		0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
		0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
		0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
		0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
		0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
		0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
		0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
		0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
		0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
		0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
		0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
		0x43, 0x83, 0x41, 0x81, 0x80, 0x40
	};

	unsigned char uchCRCHi = 0xFF;
	unsigned char uchCRCLo = 0xFF;
	unsigned char uIndex;
	while (usDataLen--)
	{
		uIndex = uchCRCHi ^ *puchMsg++;
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
	}
	return (uchCRCHi << 8 | uchCRCLo);
}
//16进制转换
char CmodbusDlg::HexChar(char c)
{
	// TODO: 在此处添加实现代码.
	if ((c >= '0') && (c <= '9'))
		return c - '0';//16进制中的，字符0-9转化成10进制，还是0-9
	else if ((c >= 'A') && (c <= 'F'))
		return c - 'A' + 10;//16进制中的A-F，分别对应着11-16
	else if ((c >= 'a') && (c <= 'f'))
		return c - 'a' + 10;//16进制中的a-f，分别对应也是11-16，不区分大小写
	else
		return 0x20; // 其他返回0x10
}
void CmodbusDlg::StringtoHex(BYTE * GB, int glen, BYTE* SB, int* slen)
{
	// TODO: 在此处添加实现代码.
	int i; //遍历输入的字符串
	int a = 0;
	char temp; //接收字符，用来判断是否为空格，若是则跳过
	char temp1, temp2; //接收一个字节的两个字符 例如EB，则temp1='E',temp2 = 'B'
	*slen = 0; //输出的16进制字符串长度
	for (i = 0; i < glen; i++)
	{
		temp = GB[i];
		if (temp == ' ')
			continue;

		if (a == 0)
			temp1 = GB[i];
		if (a == 1)
			temp2 = GB[i];
		a++;

		if (a == 2)
		{
			a = 0;
			temp1 = HexChar(temp1);
			//temp1 = temp1 - '0';
			//if (temp1>10)
		   // temp1 = temp1 -7;
			temp2 = HexChar(temp2);
			//temp2 = temp2 - '0';
			//if (temp2>10)
			//temp2 = temp2 -7;

			SB[*slen] = temp1 * 16 + temp2;
			(*slen)++;
		}
	}
}
//数据接收
void CmodbusDlg::OnReceive()
{
	// TODO: 在此处添加实现代码.
	unsigned char SendFreqData[50];
	unsigned char RecCrcData[50];
	DWORD MidData;
	CString RecStr;
	CString strtemp;
	//设置单次发送不计时T2,非单次发送计时T2
	if (SendOnce == false)
	{
		//MessageBox(_T("计时"));
		m_CadT2 = GetTickCount();
	}

	if ((m_CadT2 - m_CadT1) > 50)
		OverTime = true;
	//CString tt;
	//tt.Format(_T("2:%d"), T2 - T1);//前后之差即程序运行时间  
	//MessageBox(tt);


	//每次传进来的str都是新的 第二次的str是直接把所有的数据读到一块儿了
	char * str = NULL;
	str = new char[1024];

	//std::shared_ptr<char> str(new char[1024]);

	int iRet = m_SerialPort.readAllData(str); //06发过来的数据长度为8 接收到的数据是没错的

	

	//CString msg;
 //   //%02X为16进制显示  %d十进制 %s 字符串
 //   msg.Format(_T("%d"), iRet);
 //   MessageBox(msg);
	//Sleep(1000);
	if (iRet > 0)
	{
		//MessageBox(_T("1"));
		if (iRet == 7)
		{
			//计算收到的数据的CRC值
			SendFreqData[0] = str[0];
			SendFreqData[1] = str[1];
			SendFreqData[2] = str[2];
			SendFreqData[3] = str[3];
			SendFreqData[4] = str[4];
			MidData = CRC16(SendFreqData, 5);
			SendFreqData[5] = MidData >> 8;
			SendFreqData[6] = MidData - 256 * SendFreqData[4];
			//这部分是用来显示在接受栏中 确保显示出来的接收数据的原生性
			RecCrcData[0] = str[0];
			RecCrcData[1] = str[1];
			RecCrcData[2] = str[2];
			RecCrcData[3] = str[3];
			RecCrcData[4] = str[4];
			// CString msg;
			////%02X为16进制显示  %d十进制 %s 字符串
			// msg.Format(_T("%02X"), RecCrcData[0]);
			// MessageBox(msg);
			//转为无符号
			RecCrcData[5] = str[5];
			RecCrcData[6] = str[6];
			if (SendFreqData[5] == RecCrcData[5] && SendFreqData[6] == RecCrcData[6])
			{
			    //MessageBox(_T("相等"));
			    MidData = SendFreqData[4];
			    char MyChar[10];
			    _itoa_s(MidData, MyChar, 10);
			    //测试成功 RecStr值为1，长度为1可以进行后续判断了
			    RecStr = MyChar;

				//发送接收时间计算
				//T2 = GetTickCount();
				//CString tt;
				//tt.Format(_T("%d"), T2 - T1);//前后之差即程序运行时间  
				//MessageBox(tt);

			    if (RecStr == "1") //已经到达
			    {
			        //MessageBox(_T("相等"));
					ArriveFlag = true;
			    }
			    else if (RecStr == "0")
			    {
					ArriveFlag = false;
					
			        //一旦背板不在了，就把识别程序执行标志打成正的
					ExecuteIdentify = true;
			    }
			}
		}
		else if (iRet == 8)
		{
			SendFreqData[0] = str[0];
			SendFreqData[1] = str[1];
			SendFreqData[2] = str[2];
			SendFreqData[3] = str[3];
			SendFreqData[4] = str[4];
			SendFreqData[5] = str[5];
			MidData = CRC16(SendFreqData, 6);
			SendFreqData[6] = MidData >> 8;
			SendFreqData[7] = MidData - 256 * SendFreqData[5];

			//显示在接受栏中
			RecCrcData[0] = str[0];
			RecCrcData[1] = str[1];
			RecCrcData[2] = str[2];
			RecCrcData[3] = str[3];
			RecCrcData[4] = str[4];
			RecCrcData[5] = str[5];
			//CString msg;
			////%02X为16进制显示  %d十进制 %s 字符串
			//msg.Format(_T("%02X"), SendFreqData[7]);
			//MessageBox(msg);
			RecCrcData[6] = str[6];
			RecCrcData[7] = str[7];
			//对比发送再收回的CRC校验
			if (SendFreqData[6] == RecCrcData[6] && SendFreqData[7] == RecCrcData[7])
			{
				RecMsgFlag = true;
				RecStr = "GOOD";
				//m_CadT2 = GetTickCount();
				RecNum = RecNum + 1;
				//CString msg;
				////%02X为16进制显示  %d十进制 %s 字符串
				//msg.Format(_T("%d"), RecNum);
				//MessageBox(msg);

				//MessageBox(RecStr);
			}
			else
			{
				RecMsgFlag = false;
				//BadNum++; //问坏数大于3 在发送定时器那里进行坏数累加，不在这里累加了
						  //这里设置一个flag 大于3时 我那边停止发送
						  //出现一个flag 我那边把对应的数据发一次
						  //这里我没设置等待
			}
		}

		//把收到的数据显示出来
		/*CString RecStr((char *)str);
		m_EditReceiveCtrl.SetSel(-1, -1);
		m_EditReceiveCtrl.ReplaceSel(RecStr);*/
		CString RecStr;
		for (int k = 0; k < iRet; k++)
		{ 
			strtemp.Format(_T("%02X "), RecCrcData[k]);

			RecStr += strtemp;
		}
		RecStr += "\r\n";
		m_EditReceiveCtrl.ReplaceSel(RecStr);

		m_EditReceiveCtrl.SetSel(-1, -1);
		//this->SetDlgItemTextW(IDC_EDIT2, m_EditReceive);//将m_EditReceive内容显示到ID为IDC_EDIT1的编辑框的最后位置
		//m_EditReceiveCtrl.LineScroll(m_EditReceiveCtrl.GetLineCount() - 1, 0);//将垂直滚动条滚动到最后一
		
	}
	delete []str;
}

//数据发送函数
//CommTypeIn：0是读寄存器，1是写寄存器
//发送边框尺寸数据即 SendData(1,10,10000)
//最高位做判断位 0-32767为正 32768-65535为负
void CmodbusDlg::SendData(int CommTypeIn, WORD DownAdd, DWORD DownData)
{
	// TODO: 在此处添加实现代码.
	unsigned char SendData[200];
	unsigned short CRCData;
	//CByteArray SendArray;
	char SendArray[8];
	DWORD MidData;
	BYTE StrLength;

	SendData[0] = 01;
	if (CommTypeIn == 0)
		SendData[1] = 3;
	else
		SendData[1] = 6;
	MidData = DownAdd;
	SendData[2] = (unsigned char)(MidData >> 8);
	SendData[3] = (unsigned char)(MidData - 256 * SendData[2]);
	MidData = DownData;
	SendData[4] = (unsigned char)(MidData >> 8);
	SendData[5] = (unsigned char)(MidData - 256 * SendData[4]);
	//CRC校验
	CRCData = CRC16(SendData, 6);
	SendData[6] = CRCData >> 8;
	SendData[7] = CRCData - 256 * SendData[6];
	StrLength = 8;

	//CString msg;
	////%02X为16进制显示  %d十进制 %s 字符串
	//msg.Format(_T("%02X"), SendData[3]);
	//MessageBox(msg);

	/*SendArray.RemoveAll();
	SendArray.SetSize(StrLength);*/
	//把待发送数据存入数组中

	//CString msg;
 //   //%02X为16进制显示  %d十进制 %s 字符串
 //   msg.Format(_T("%02X"), SendData[3]);
 //   MessageBox(msg);

	for (int Circle = 0; Circle < StrLength; Circle++)
		SendArray[Circle] = SendData[Circle];

	//int len1 = SendArray.GetSize();
	//CString temp('x', len1);
	//for (int i = 0; i < len1; i++)
	//{m_SerialPort
	//	temp.SetAt(i, SendArray.GetAt(i));
	//}

	//int n = temp.GetLength();//n= 8
	//int len = WideCharToMultiByte(CP_ACP, 0, temp, temp.GetLength(), NULL, 0, NULL, NULL);//len = 8
	//char *m_str = new char[len + 1];
	//WideCharToMultiByte(CP_ACP, 0, temp, temp.GetLength(), m_str, len, NULL, NULL);
	//m_str[len + 1] = '\0';

	m_SerialPort.writeData(SendArray, 8);
	//m_SerialPort.writeData(m_str, len);
}

//定时发送
void CmodbusDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//CPublic::Port = portName;
	//ReadArrive();  
	SetTimer(1, 200, NULL);
	//if (CPublic::test1 == false)
	//{
	//	//CString msg;
	//	////%02X为16进制显示  %d十进制 %s 字符串
	//	//msg.Format(_T("%d"), CPublic::test1);
	//	//MessageBox(msg);

	//	SendData(1, 100, 100);
	//}

	

	//SendGlueData();
	//T1 = GetTickCount();//程序段开始前取得系统运行时间(ms) 
	//SendData(1, 103, 100);
	/*SendData(1, 100, 100);
	Sleep(35);
	SendData(1, 101, 100);*/
}

//
void CmodbusDlg::ReadArrive()
{
	// TODO: 在此处添加实现代码.
	SendData(0, 95, 1);
}



void CmodbusDlg::SendGlueData()
{
	// TODO: 在此处添加实现代码.
	//WORD temp[200];//发送数据存储空间
	//int i = 0;//把一行数据读到寄存器时用到的
	//CString w[1024];
	//CString str;//用于接收分割字符的临时变量


	//CString strline;//定义一个变量用于接收读取的一行内容
	//CStdioFile file;//定义一个CStdioFile类的对象 file
	////GlueX1 = _wtoi(GlueAdd); CString转word
	//std::vector<CString> vecResult;
	//std::vector<CString> strTmp;
	////清除元素，并收回内存 std::vector <CString>().swap(strTmp);
	////vecResult.clear()清除元素但不收回空间
	//BOOL flag = file.Open(_T(".\\path.txt"), CFile::modeRead);//open函数需要传两个参数，前一个是文件路径，后一个是文件的打开模式
	//if (flag == FALSE)
	//{
	//	//MessageBox(_T("文件打开失败"));
	//}
	//while (file.ReadString(strline))
	//{
	//	vecResult.push_back(strline);
	//}
	//file.Close();
	//for (std::vector<CString>::iterator it = vecResult.begin(); it != vecResult.end(); it++)
	//{
	//	w[i] = vecResult[i];
	//	i = i + 1;
	//}
	////分割字符部分//

	//for (int j = 0; j < i; j++)
	//{
	//	int curPos = 0;
	//	str = w[j].Tokenize(_T(" "), curPos);
	//	while (str.Trim() != _T(""))
	//	{
	//		//输入寄存器里的喷胶数据5个一轮 0-4为第一组，但是注意多出来的空的为00
	//		strTmp.push_back(str);
	//		str = w[j].Tokenize(_T(" "), curPos);
	//	}
	//}
	////把容器的
	//vecGlueNum = strTmp.size();
	////第一列的值
	//for (int j = 0, a = 0; j < (strTmp.size() / 5) + 1; j++)
	//{
	//	CString name;
	//	name.Format(_T("第%d条"), j + 1);
	//	m_GlueList.InsertItem(j, name);
	//	
	//	m_GlueList.SetItemText(j, 1, strTmp[a]);
	//	m_GlueList.SetItemText(j, 2, strTmp[a + 1]);
	//	m_GlueList.SetItemText(j, 3, strTmp[a + 2]);
	//	m_GlueList.SetItemText(j, 4, strTmp[a + 3]);
	//	m_GlueList.SetItemText(j, 5, strTmp[a + 4]);
	//	a = j * 5;
	//}

	////通过容器的容量size即当前容器里边的数据多少
	//for (int j = 0; j < strTmp.size(); j++)
	//{

	//	//把负的数据处理变成范围内的数据
	//	if (strTmp[j].GetAt(0) == '-')
	//	{
	//		strTmp[j].Remove('-');
	//		temp[j] = _wtoi(strTmp[j]);
	//		temp[j] = temp[j] + 32767;
	//	}
	//	else
	//		temp[j] = _wtoi(strTmp[j]);

	//	GlueTemp[j] = temp[j];
	//	//CString msg;
	//	////%02X为16进制显示  %d十进制 %s 字符串
	//	//msg.Format(_T("%02X"), temp[j]);
	//	//MessageBox(msg);
	//}
	//
	//SetTimer(2, 50, NULL);
	//发送
	//for (int j = 0; j < strTmp.size(); j++)
	//{
	//	SendData(1, j + 100, temp[j]);

	//	Sleep(50);//极限了

	//	
	//	/*SendData(1, j+101, temp[j+1]);
	//	Sleep(1000);
	//	SendData(1, j+102, temp[j+2]);
	//	Sleep(1000);
	//	SendData(1, j+103, temp[j+3]);
	//	Sleep(1000);
	//	SendData(1, j+104, temp[j+4]);
	//	Sleep(1000);*/
	//	发送的胶条数量加1
	//	没收到或者收到超时
	//	if(OverTime == true)

	//	if (OverTime == true || RecMsgFlag == false)
	//	{
	//		int n = 0;
	//		//while ((OverTime == true || RecMsgFlag == false) && (BadNum <= 3 || n < 3))
	//		while(OverTime == true)
	//		{
	//			SendData(1, j + 100, temp[j]);
	//			Sleep(35);
	//			BadNum = BadNum + 1;

	//			//CString msg;
	//			////%02X为16进制显示  %d十进制 %s 字符串
	//			//msg.Format(_T("重发第%d次"), BadNum);
	//			//MessageBox(msg);

	//			if (BadNum >= 3)
	//			{
	//				break;
	//			}
	//		}

	//	}
	//	if (BadNum >= 3)
	//	{
	//		break;
	//	}

	//	SendGlueNum++;
	//}
}


void CmodbusDlg::SendLocationData()
{
	// TODO: 在此处添加实现代码.
	//WORD temp[100];//发送数据存储空间
	//int i = 0;//把一行数据读到寄存器时用到的
	//CString w[1024];
	//CString str;//用于接收分割字符的临时变量

	//CString strline;//定义一个变量用于接收读取的一行内容
	//CStdioFile file;//定义一个CStdioFile类的对象 file
	////GlueX1 = _wtoi(GlueAdd); CString转word
	//std::vector<CString> vecLocationResult;
	//std::vector<CString> LocationTmp;
	////清除元素，并收回内存 std::vector <CString>().swap(strTmp);
	////vecResult.clear()清除元素但不收回空间
	//BOOL flag = file.Open(_T("D:\\MyCodeWorkPlace\\SerialPort\\Debug\\SendLocationData.txt"), CFile::modeRead);//open函数需要传两个参数，前一个是文件路径，后一个是文件的打开模式
	//if (flag == FALSE)
	//{
	//	//MessageBox(_T("文件打开失败"));
	//}
	//while (file.ReadString(strline))
	//{
	//	vecLocationResult.push_back(strline);
	//}
	//file.Close();
	//for (std::vector<CString>::iterator it = vecLocationResult.begin(); it != vecLocationResult.end(); it++)
	//{
	//	w[i] = vecLocationResult[i];
	//	i = i + 1;
	//}
	////分割字符部分//

	//for (int j = 0; j < i; j++)
	//{
	//	int curPos = 0;
	//	str = w[j].Tokenize(_T(" "), curPos);
	//	while (str.Trim() != _T(""))
	//	{
	//		//输入寄存器里的定位数据3个一轮 0-2为第一组，但是注意多出来的空的为00
	//		LocationTmp.push_back(str);
	//		str = w[j].Tokenize(_T(" "), curPos);
	//	}
	//}
	//for (int j = 0; j < LocationTmp.size() / 3; j++)
	//{
	//	CString name;
	//	name.Format(_T("坐标%d"), j + 1);
	//	m_IdentifyList.InsertItem(j, name);
	//	m_IdentifyList.SetItemText(j, 1, LocationTmp[j]);
	//	m_IdentifyList.SetItemText(j, 2, LocationTmp[j + 1]);
	//	m_IdentifyList.SetItemText(j, 3, LocationTmp[j + 2]);

	//}
	////通过容器的容量size即当前容器里边的数据多少
	//for (int j = 0; j < LocationTmp.size(); j++)
	//{
	//	if (LocationTmp[j].GetAt(0) == '-')
	//	{
	//		LocationTmp[j].Remove('-');
	//		temp[j] = _wtoi(LocationTmp[j]);
	//		temp[j] = temp[j] + 32767;
	//	}
	//	temp[j] = _wtoi(LocationTmp[j]);
	//}
	////发送
	//for (int j = 0; j < LocationTmp.size(); j++)
	//{
	//	SendData(1, j + 90, temp[j]);
	//	Sleep(35);//极限了

	//	//发送的胶条数量加1
	//	if (OverTime == true || RecMsgFlag == false)
	//	{
	//		int n = 0;
	//		while ((OverTime == true || RecMsgFlag == false) && (BadNum <= 3 || n < 3))
	//		{
	//			SendData(1, j + 10, temp[j]);
	//			Sleep(35);
	//			n = n + 1;
	//		}

	//	}
	//	if (BadNum >= 3)
	//	{
	//		break;
	//	}

	//	SendLocationNum++;
	//}
}


void CmodbusDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
		//一直读200ms，为true到位，false则不到位
		//到位之后就可以发送尺寸数据

		case 1:
		{
			SendOnce = true;
			ReadArrive();

			//背板到达，识别程序开始执行，标志位变成true.这里可以直接替换为等待IdentifyDone完成，直接执行发送数据
			//的命令，进入判断之后立刻把标志位打为false，防止多次重复发送
			if (ArriveFlag == true && ExecuteIdentify == true)
			{
				//CPublic::ExecuteIdentify = false;
			
				//识别程序启动
					//if(done)
						//sendlocation
				//MessageBox(_T("6"));

			}
			break;
		}
		//case 2:
		//{
		//	//CString msg;
		//	////%02X为16进制显示  %d十进制 %s 字符串
		//	//msg.Format(_T("定时器2执行"));
		//	//MessageBox(msg);
		//	//判断的是上一组已发送的数据的标志，如果没有超时且上一组数据CRC校验正确的话，那么可以进行本次发送
		//	T1 = GetTickCount();
		//	if (T2 != 0 && OverTime == false && RecMsgFlag == true)
		//	{
		//		//如果上一个数据发送成功，那么将BadNum置为0
		//		BadNum = 0;
		//		if (locGlueNum < vecGlueNum)
		//		{

		//			SendData(1, locGlueNum + 100, GlueTemp[locGlueNum]);
		//			locGlueNum++;
		//		}
		//		else
		//		{
		//			//发送完毕之后，可以考虑每次按下发送键的时候把这个置为0，把定位数据置为0，方便下次发送
		//			//locGlueNum = 0;
		//			KillTimer(2);
		//		}
		//	}
		//	//这里是上一组信息发送有误的情况
		//	else
		//	{
		//		//进入错误阶段，首先错误数累加，每个数据有两次重发机会，
		//		BadNum++;
		//		if (BadNum < 3)
		//		{
		//			//第一个数据出现错误与后边的数据出现错误是一样的处理措施
		//			//先减1发送前一个数据
		//			locGlueNum = locGlueNum - 1;
		//			SendData(1, locGlueNum + 100, GlueTemp[locGlueNum]);
		//			//发送完之后做加一处理
		//			locGlueNum++;
		//		}
		//		else
		//		{

		//			//停止发送
		//			KillTimer(2);
		//			//报错
		//			CString msg;

		//			//%02X为16进制显示  %d十进制 %s 字符串
		//			msg.Format(_T("第%d个数据发送错误，终止发送！"), locGlueNum);
		//			MessageBox(msg);

		//		}
		//	}
		//	break;
		//}
	}
	__super::OnTimer(nIDEvent);
}





void CmodbusDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	m_layoutMod.OnSize(cx, cy);
}


void CmodbusDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	__super::OnSizing(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
}





void CmodbusDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_SerialPort.close();
	__super::OnClose();
}


void CmodbusDlg::OnBnClickedButtonClean()
{
	// TODO: 在此添加控件通知处理程序代码
	m_EditReceive = _T("");
	/*m_EditReceive = _T("");
	UpdateData(FALSE);*/
	UpdateData(FALSE);
}


void CmodbusDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
	CRect rect;
	GetClientRect(rect);
	
	dc.FillSolidRect(rect, RGB(240, 240, 220));
	//dc.FillSolidRect(rect, RGB(125, 125, 255));

	CDialogEx::OnPaint();
					   // 不为绘图消息调用 __super::OnPaint()
}


BOOL CmodbusDlg::PreTranslateMessage(MSG* pMsg)
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
