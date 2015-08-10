
// UseSScomDllExampleDlg.h : 头文件
//

#pragma once

#define WM_RECIVESTRING WM_USER + 100

// CUseSScomDllExampleDlg 对话框
class CUseSScomDllExampleDlg : public CDialogEx
{
// 构造
public:
	CUseSScomDllExampleDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_USESSCOMDLLEXAMPLE_DIALOG };

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
private:
	BOOL LookForSerialPortOtherFunc(void);
public:
	afx_msg void OnBnClickedButton1();
protected:
	afx_msg LRESULT OnRecivestring(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
private:
	UINT m_RecvNumber;
};
