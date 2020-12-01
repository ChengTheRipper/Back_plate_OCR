// CchagePwdDlg.cpp: 实现文件
//

#include "pch.h"
#include "HX.h"
#include "CchagePwdDlg.h"
#include "afxdialogex.h"
#include "InfoFile.h"

// CchagePwdDlg 对话框

IMPLEMENT_DYNAMIC(CchagePwdDlg, CDialogEx)

CchagePwdDlg::CchagePwdDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHANGEPWD, pParent)
	, m_oldPwd(_T(""))
	, m_newPwd(_T(""))
	, m_surePwd(_T(""))
{

}

CchagePwdDlg::~CchagePwdDlg()
{
}

void CchagePwdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_oldPwd);
	DDX_Text(pDX, IDC_EDIT3, m_newPwd);
	DDX_Text(pDX, IDC_EDIT4, m_surePwd);
}


BEGIN_MESSAGE_MAP(CchagePwdDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CchagePwdDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CchagePwdDlg 消息处理程序


void CchagePwdDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CInfoFile file;
	CString name, pwd;
	file.ReadLogin(name, pwd);

	//修改密码功能
	if (m_oldPwd.IsEmpty() || m_newPwd.IsEmpty() || m_surePwd.IsEmpty())
	{
		MessageBox(_T("输入的内容不能为空！"));
		return;
	}

	//新密码和确定密码不一致
	if (m_newPwd != m_surePwd)
	{
		MessageBox(_T("新密码与确定密码要一致！"));
		return;
	}

	//旧密码与本地的不一致
	if (pwd != m_oldPwd)
	{
		MessageBox(_T("旧密码输入错误！"));
		return;
	}
	//旧密码与新密码不能一致
	if (pwd == m_newPwd)
	{
		MessageBox(_T("新密码与旧密码不能一致！"));
		return;
	}

	//CString转char*
	CStringA tmp;
	CStringA tmp2;
	tmp = name;
	tmp2 = m_newPwd;
	file.WritePwd(tmp.GetBuffer(), tmp2.GetBuffer());
	MessageBox(_T("修改成功！"));

	//清空内容
	m_oldPwd.Empty();
	m_surePwd.Empty();
	m_newPwd.Empty();
	UpdateData(FALSE);
}


BOOL CchagePwdDlg::PreTranslateMessage(MSG* pMsg)
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
