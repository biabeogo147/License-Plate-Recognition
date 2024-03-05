
// License Plate RecognitionDlg.h : header file
//

#pragma once

#include "afxwin.h"
#include "LprCore.h"

#include <opencv2/core/core.hpp>


// CLicensePlateRecognitionDlg dialog
class CLicensePlateRecognitionDlg : public CDialogEx
{
// Construction
public:
	CLicensePlateRecognitionDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LICENSE_PLATE_RECOGNITION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnLoadImage();
	afx_msg void OnBnClickedShowResult();
protected:
	CStatic pic_control;

private:
	Mat src;
	Mat plate;
	LprCore lpr;
	Mat character;
	CString file_name;
	string text_recognition;
public:
	CStatic text_result;
};
