// CLoginDlg.cpp: 实现文件
//

#include "pch.h"
#include "HX.h"
#include "CLoginDlg.h"
#include "afxdialogex.h"
#include "InfoFile.h"
#include "CchagePwdDlg.h"

//全局变量 管理员登录标志位
bool LoginFlag = false;

// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIN, pParent)
	, m_user(_T(""))
	, m_pwd(_T(""))
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USER, m_user);
	DDX_Text(pDX, IDC_EDIT_PWD, m_pwd);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CLoginDlg::OnBnClickedButtonLogin)
	
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CLoginDlg::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_SETPWD, &CLoginDlg::OnBnClickedButtonSetpwd)
END_MESSAGE_MAP()


// CLoginDlg 消息处理程序


void CLoginDlg::OnBnClickedButtonLogin()
{
	// TODO: 在此添加控件通知处理程序代码
	//登录按钮点击事件
	UpdateData(true);//控件的内容同步到变量上

	if (m_pwd.IsEmpty() || m_user.IsEmpty())
	{
		MessageBox(_T("输入的内容不能为空"));
		return;
	}

	//获取正确的值
	CInfoFile file;
	CString name, pwd;
	file.ReadLogin(name, pwd);

	if (name == m_user) //用户名一致
	{
		if (pwd == m_pwd)
		{
			//管理员登陆标志位打为正，管理员登陆成功
			LoginFlag = true;
			//关闭当前的对话框
			CDialog::OnCancel();
		}
		else
		{
			MessageBox(_T("密码有误"));
		}
	}
	else
	{
		MessageBox(_T("用户名有误"));
	}
}




//重写onok避免按回车使窗口消失
void CLoginDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}


void CLoginDlg::OnBnClickedButtonCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}


BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//默认登陆信息
	CInfoFile file;
	CString name, pwd;
	file.ReadLogin(name, pwd);
	//默认用户名
	m_user = name;
	//密码
	//m_pwd = pwd;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CLoginDlg::OnBnClickedButtonSetpwd()
{
	// TODO: 在此添加控件通知处理程序代码
	CchagePwdDlg m_Dlg_pwd;
	m_Dlg_pwd.DoModal();
}


BOOL CLoginDlg::PreTranslateMessage(MSG* pMsg)
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
