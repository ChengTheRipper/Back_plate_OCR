// CcadDlg.cpp: 实现文件
//

#include "pch.h"
#include "HX.h"
#include "CcadDlg.h"
#include "afxdialogex.h"

#include "drawcad.h"
#include <windows.h>
#include <memory>
#include <string>
#include <vector>

#include  "layoutinitCad.h"



#define _CRT_SECURE_NO_WARNINGS

ULONG_PTR m_gdiplusToken;
m_mfc_drawer* drawer;

//胶条数据的总数量，是从sendGlueData赋值
//一定要执行选择图纸再点发送，不然这个值不会赋予
int vecGlueNum = 0;
//这个数据暂时不用发完就清空，因为每一次都会把对应的值覆盖进去
WORD GlueTemp[200];//把胶条数据从函数里边提取出来变成全局的，用以发送
//发送胶条时计数
int locGlueNum = 0;

// CcadDlg 对话框

IMPLEMENT_DYNAMIC(CcadDlg, CDialogEx)

CcadDlg::CcadDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CAD, pParent)
{

}

CcadDlg::~CcadDlg()
{
}

void CcadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CAD_LOC, m_CadGlueList);
}


BEGIN_MESSAGE_MAP(CcadDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	
	ON_BN_CLICKED(IDC_BUTTON_CAD_OPEN, &CcadDlg::OnBnClickedButtonCadOpen)
	ON_BN_CLICKED(IDC_BUTTON_CAD_DRAW, &CcadDlg::OnBnClickedButtonCadDraw)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_SIZING()
	
	ON_BN_CLICKED(IDC_BUTTON_CAD_SEND, &CcadDlg::OnBnClickedButtonCadSend)
END_MESSAGE_MAP()


// CcadDlg 消息处理程序

BOOL CcadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	////胶条列表
	CString strglue[] = { _T("条数"),_T("X"),_T("Y"),_T("是否喷胶") };
	for (int i = 0; i < 4; i++)
	{
		//设置表头 索引 内容 对齐方式 列宽度
		m_CadGlueList.InsertColumn(i, strglue[i], LVCFMT_LEFT, 100);
	}
	m_CadGlueList.GetScrollPos(SB_VERT); //设置垂直滚动条
	DWORD dwStyle1 = m_CadGlueList.GetExtendedStyle();
	dwStyle1 |= LVS_EX_GRIDLINES;
	m_CadGlueList.SetExtendedStyle(dwStyle1);//设置扩展风格

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);


	InitLayoutCad(m_layoutCad, this);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


HBRUSH CcadDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	//return hbr;
	return (HBRUSH)::GetStockObject(WHITE_BRUSH);
}







unsigned short CcadDlg::CRC16(unsigned char* puchMsg, unsigned short usDataLen)
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


void CcadDlg::SendData(int CommTypeIn, WORD DownAdd, DWORD DownData)
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
	//{
	//	temp.SetAt(i, SendArray.GetAt(i));
	//}

	//int n = temp.GetLength();//n= 8
	//int len = WideCharToMultiByte(CP_ACP, 0, temp, temp.GetLength(), NULL, 0, NULL, NULL);//len = 8
	//char *m_str = new char[len + 1];
	//WideCharToMultiByte(CP_ACP, 0, temp, temp.GetLength(), m_str, len, NULL, NULL);
	//m_str[len + 1] = '\0';

	CmodbusDlg *pdlg = CmodbusDlg::pModbusdlg;
	pdlg->m_SerialPort.writeData(SendArray, 8);

	//CPublic::m_SerialPort.writeData(SendArray, 8);
}




void CcadDlg::OnBnClickedButtonCadOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	CString filename;
	CFileDialog fileDlg(true, _T("dwg"), _T("*.dwg"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T(" dwg files (*.dwg)|*.dwg|All Files (*.*)|*.*||"), NULL);
	if (fileDlg.DoModal() == IDOK)    //弹出对话框  
	{
		filename = fileDlg.GetPathName();//得到完整的文件名和目录名拓展名  
		GetDlgItem(IDC_EDIT_CAD_PATH)->SetWindowText(filename);//将路径显示  
		//CString filename = fileDlg.GetFileName();
	}
	else
		return;

	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = this->m_hWnd;
	ShExecInfo.lpVerb = _T("open");
	ShExecInfo.lpFile = _T("libdwg_exmaple.exe");
	ShExecInfo.lpParameters = filename;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOWNORMAL;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	if (ShExecInfo.hProcess != NULL)
	{
		//等待程序运行完毕
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
		TerminateProcess(ShExecInfo.hProcess, 0);
		ShExecInfo.hProcess = NULL;

	}

	m_CadGlueList.DeleteAllItems(); //清空所有表项

	WORD temp[200];//发送数据存储空间
	int i = 0;//把一行数据读到寄存器时用到的
	CString w[1024];
	CString str;//用于接收分割字符的临时变量
	CString decimals;

	CString strline;//定义一个变量用于接收读取的一行内容
	CStdioFile file;//定义一个CStdioFile类的对象 file
	//GlueX1 = _wtoi(GlueAdd); CString转word
	std::vector<CString> vecResult;
	std::vector<CString> strTmp;
	std::vector<CString> strDecimals;

	//清除元素，并收回内存 std::vector <CString>().swap(strTmp);
	//vecResult.clear()清除元素但不收回空间
	BOOL flag = file.Open(_T(".\\path.txt"), CFile::modeRead);//open函数需要传两个参数，前一个是文件路径，后一个是文件的打开模式
	if (flag == FALSE)
	{
		//MessageBox(_T("文件打开失败"));
	}
	while (file.ReadString(strline))
	{
		vecResult.push_back(strline);
	}
	file.Close();
	for (std::vector<CString>::iterator it = vecResult.begin(); it != vecResult.end(); it++)
	{
		w[i] = vecResult[i];
		i = i + 1;
	}
	//分割字符部分//

	for (int j = 0; j < i; j++)
	{
		int curPos = 0;
		str = w[j].Tokenize(_T(" "), curPos);
		while (str.Trim() != _T(""))
		{
			//输入寄存器里的喷胶数据5个一轮 0-4为第一组，但是注意多出来的空的为00
			strTmp.push_back(str);
			str = w[j].Tokenize(_T(" "), curPos);
		}
	}
	//把容器的
	vecGlueNum = strTmp.size();
	//通过容器的容量size即当前容器里边的数据多少
	for (int j = 0; j < strTmp.size(); j++)
	{


		temp[j] = _wtoi(strTmp[j]);

		GlueTemp[j] = temp[j];
		//CString msg;
		////%02X为16进制显示  %d十进制 %s 字符串
		//msg.Format(_T("%02X"), temp[j]);
		//MessageBox(msg);
	}

	//把容器里的数据除以10正常化
	for (int j = 0; j < strTmp.size(); j++)
	{
		//(.1表示小数点后留1位，小数点后超过1位则四舍五入)
		decimals.Format(_T("%.1lf"), _ttof(strTmp[j]) / 10);
		strDecimals.push_back(decimals);
	}
	//第一列的值
	for (int j = 0, a = 0; j < (strDecimals.size() / 3); j++)
	{
		CString name;
		name.Format(_T("第%d条"), j + 1);
		m_CadGlueList.InsertItem(j, name);
		
		a = j * 3;

		m_CadGlueList.SetItemText(j, 1, strDecimals[a]);
		m_CadGlueList.SetItemText(j, 2, strDecimals[a + 1]);
		m_CadGlueList.SetItemText(j, 3, strDecimals[a + 2]);
		/*m_CadGlueList.SetItemText(j, 4, strDecimals[a + 3]);
		m_CadGlueList.SetItemText(j, 5, strTmp[a + 4]);*/
		
		//a = j * 5;
	}

	

}


void CcadDlg::OnBnClickedButtonCadDraw()
{
	// TODO: 在此添加控件通知处理程序代码
	drawer = new m_mfc_drawer(this, IDC_STATIC_CAD_PIC);
	read_cad(&(drawer->a1));//给drawer赋值
	drawer->clear();
	SetTimer(1, 200, NULL);
}


void CcadDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
		case 1:
		{
			if (!drawer->draw_line_animated())
			{
				KillTimer(1);
				delete drawer;
			}
			break;
		}
		//发送图纸定时器
		case 2:
		{
			SendOnce = false;
			//CString msg;
			////%02X为16进制显示  %d十进制 %s 字符串
			//msg.Format(_T("定时器2执行"));
			//MessageBox(msg);
			//判断的是上一组已发送的数据的标志，如果没有超时且上一组数据CRC校验正确的话，那么可以进行本次发送
			m_CadT1 = GetTickCount();
			if (m_CadT2 != 0 && OverTime == false && RecMsgFlag == true)
			{
				//如果上一个数据发送成功，那么将BadNum置为0
				BadNum = 0;
				if (locGlueNum < vecGlueNum)
				{

					SendData(1, locGlueNum + 100, GlueTemp[locGlueNum]);
					locGlueNum++;
				}
				else
				{
					SendData(1, 80, locGlueNum / 3);
					Sleep(30);
					SendData(1, 85, 1);
					//发送完毕之后，可以考虑每次按下发送键的时候把这个置为0，把定位数据置为0，方便下次发送
					locGlueNum = 0;
					KillTimer(2);
				}
			}
			//这里是上一组信息发送有误的情况
			else
			{
				//进入错误阶段，首先错误数累加，每个数据有两次重发机会，
				BadNum++;
				if (BadNum < 3)
				{
					//第一个数据出现错误与后边的数据出现错误是一样的处理措施
					//先减1发送前一个数据
					locGlueNum = locGlueNum - 1;
					SendData(1, locGlueNum + 100, GlueTemp[locGlueNum]);
					//发送完之后做加一处理
					locGlueNum++;
				}
				else
				{

					//停止发送
					KillTimer(2);
					//报错
					CString msg;

					//%02X为16进制显示  %d十进制 %s 字符串
					msg.Format(_T("第%d个数据发送错误，终止发送！"), locGlueNum);
					MessageBox(msg);

				}
			}
			break;
		}
	}
	

	__super::OnTimer(nIDEvent);
}


void CcadDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	m_layoutCad.OnSize(cx, cy);
}


void CcadDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	__super::OnSizing(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
}





void CcadDlg::OnBnClickedButtonCadSend()
{
	// TODO: 在此添加控件通知处理程序代码
	OverTime = false;
	CString temp;
	CmodbusDlg *pdlg = CmodbusDlg::pModbusdlg;
	pdlg->m_OpenCloseCtrl.GetWindowText(temp);///获取按钮的文本
	//UpdateData(true);
	if (temp == _T("打开串口"))///表示点击后是"关闭串口"，也就是已经关闭了串口
	{
		AfxMessageBox(_T("请先打开串口！"));
	}
	else
		SetTimer(2, 50, NULL);
}


BOOL CcadDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此处添加实现代码.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)    //屏蔽回车和ESC  
			return TRUE;
		if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4)  //屏蔽ALT+F4
			return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);  //别忘了这句 ,CXXXDlg类的基类是CDialogEx,如果是CDialog则返回CDialog
}
