
// UseSScomDllExampleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UseSScomDllExample.h"
#include "UseSScomDllExampleDlg.h"
#include "afxdialogex.h"
#include "Winspool.h"
#include "Windows.h"
#include "SScomdll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUseSScomDllExampleDlg 对话框



CUseSScomDllExampleDlg::CUseSScomDllExampleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUseSScomDllExampleDlg::IDD, pParent)
	, m_RecvNumber(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUseSScomDllExampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_RecvNumber);
}

BEGIN_MESSAGE_MAP(CUseSScomDllExampleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CUseSScomDllExampleDlg::OnBnClickedButton1)
	ON_MESSAGE(WM_RECIVESTRING, &CUseSScomDllExampleDlg::OnRecivestring)
	ON_BN_CLICKED(IDC_BUTTON2, &CUseSScomDllExampleDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CUseSScomDllExampleDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CUseSScomDllExampleDlg 消息处理程序

BOOL CUseSScomDllExampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	/*if (!LookForSerialPortOtherFunc()){
		MessageBox("查找端口函数异常！");
	}*/

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUseSScomDllExampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUseSScomDllExampleDlg::OnPaint()
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
HCURSOR CUseSScomDllExampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CUseSScomDllExampleDlg::LookForSerialPortOtherFunc(void)
{
	BOOL retValue = FALSE;
	try{
		CComboBox* pListBox = (CComboBox*)GetDlgItem(IDC_COMBO1);
		int i = 0;
		int nSerialPortNum = 0;
		CString strSerialList[256];
		LPBYTE pBite = NULL;
		DWORD pcdNeeded = 0;
		DWORD pcReturned = 0;

		nSerialPortNum = 0;

		EnumPorts(NULL,2,pBite,0,&pcdNeeded,&pcReturned);
		pBite = new BYTE[pcdNeeded];

		EnumPorts(NULL,2,pBite,pcdNeeded,&pcdNeeded,&pcReturned);
		PORT_INFO_2 *pPort;
		pPort =(PORT_INFO_2*)pBite;
		for(i = 0; i < pcReturned; i++){
			CString str = pPort[i].pPortName;
			if (str.Left(3) == "COM"){
				strSerialList[nSerialPortNum] = str.Left(str.GetLength()-1);
				HANDLE hTestHandle =  CreateFile(strSerialList[nSerialPortNum],GENERIC_READ|GENERIC_WRITE,0,
					NULL,OPEN_EXISTING,NULL,NULL);
				if (hTestHandle != INVALID_HANDLE_VALUE){
					pListBox->AddString(strSerialList[nSerialPortNum]);
					nSerialPortNum++;
					CloseHandle(hTestHandle);
				}
			}
		}
		delete[] pBite;
		retValue = TRUE;
	}
	catch(...){
		retValue = FALSE;
	}
	return retValue;
}


void CUseSScomDllExampleDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	GetDlgItemText(IDC_COMBO1,str);
	if (str.GetLength() == 0){
		MessageBox("请输入串口号!");
		return;
	}
	//MessageBox(str.GetBuffer(str.GetLength()));
	INT res = Opencom(str.GetBuffer(str.GetLength()),this->m_hWnd,WM_RECIVESTRING);
	if (res != 1){
		MessageBox("打开串口失败!");
	}
	//str.Format("%d",res);
	//MessageBox(str.GetBuffer(str.GetLength()));
}


afx_msg LRESULT CUseSScomDllExampleDlg::OnRecivestring(WPARAM wParam, LPARAM lParam)
{
	CHAR* pbuf = (CHAR*)lParam;
	DWORD bufsize = *((DWORD*)wParam);
	UpdateData();
	m_RecvNumber += bufsize;
	UpdateData(FALSE);
	int count = ((CEdit*)(this->GetDlgItem(IDC_RICHEDIT21)))->GetLineCount();
	int nLastLineStart = ((CEdit*)(this->GetDlgItem(IDC_RICHEDIT21)))->LineIndex(count-1);
	int nLastLineEnd = nLastLineStart + ((CEdit*)(this->GetDlgItem(IDC_RICHEDIT21)))->LineLength(nLastLineStart);
	((CEdit*)(this->GetDlgItem(IDC_RICHEDIT21)))->SetSel(nLastLineEnd+1,nLastLineEnd+1);
	((CEdit*)(this->GetDlgItem(IDC_RICHEDIT21)))->ReplaceSel(pbuf);
	((CEdit*)(this->GetDlgItem(IDC_RICHEDIT21)))->PostMessage(WM_VSCROLL,SB_BOTTOM,0);
	return 0;
}


void CUseSScomDllExampleDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (CloseComm() == -1){
		MessageBox("CloseComm Error!!");
	}
}


void CUseSScomDllExampleDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	GetDlgItemText(IDC_EDIT1,str);
	if (str.GetLength() != 0){
		if (WriteComm(str.GetBuffer(str.GetLength()),str.GetLength()) != 1){
			MessageBox("WriteComm Error!");
		}
	}
}
