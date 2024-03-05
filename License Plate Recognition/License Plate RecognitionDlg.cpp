
// License Plate RecognitionDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "afxdialogex.h"
#include "License Plate Recognition.h"
#include "License Plate RecognitionDlg.h"

#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
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


// CLicensePlateRecognitionDlg dialog



CLicensePlateRecognitionDlg::CLicensePlateRecognitionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LICENSE_PLATE_RECOGNITION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLicensePlateRecognitionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_CONTROL, pic_control);
	DDX_Control(pDX, IDC_LICENSE_PLATE, text_result);
}

BEGIN_MESSAGE_MAP(CLicensePlateRecognitionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_LOAD_IMAGE, &CLicensePlateRecognitionDlg::OnBnClickedBtnLoadImage)
	ON_BN_CLICKED(IDC_SHOW_RESULT, &CLicensePlateRecognitionDlg::OnBnClickedShowResult)
END_MESSAGE_MAP()


// CLicensePlateRecognitionDlg message handlers

BOOL CLicensePlateRecognitionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	CString filePath(_T("D:\\Image Processing\\svm.xml"));
	if (GetFileAttributes(filePath) == INVALID_FILE_ATTRIBUTES) {
		lpr.train_lpr.train_model();
	}
	lpr.set_svm_model("D:\\Image Processing\\svm.xml");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLicensePlateRecognitionDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLicensePlateRecognitionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLicensePlateRecognitionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLicensePlateRecognitionDlg::OnBnClickedBtnLoadImage()
{
	static CString Filter = _T("JPEG Files (*.jpg)|*.jpg|All Files (*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("jpg"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, Filter, NULL);
	if (dlg.DoModal() == IDOK)
	{
		file_name = dlg.GetPathName();
		string file_name2 = CT2A(file_name);
		src = imread(file_name2);

		text_result.SetWindowTextW(file_name);

		if (src.empty())
			return;

		lpr.set_image(src);

		CImage image;
		image.Load(file_name);

		CRect rect;
		pic_control.GetClientRect(&rect);

		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap(CDC::FromHandle(::GetDC(NULL)), rect.Width(), rect.Height());
		
		CDC memDC;
		memDC.CreateCompatibleDC(CDC::FromHandle(::GetDC(NULL)));
		
		CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
		image.StretchBlt(memDC.m_hDC, 0, 0, rect.Width(), rect.Height(), 0, 0, image.GetWidth(), image.GetHeight(), SRCCOPY);
		memDC.SelectObject(pOldBitmap);

		pic_control.SetBitmap((HBITMAP)bitmap.Detach());
	}
}

void CLicensePlateRecognitionDlg::OnBnClickedShowResult()
{
	// TODO: Add your control notification handler code here
	if (lpr.get_image().empty())
		return;

	Mat disp_plate, disp_character;
	lpr.do_process();

	vector<string> text = lpr.get_text_recognition();
	if (text.size() > 0)
	{
		text_recognition = text[0];
		wstring cvt = wstring(text_recognition.begin(), text_recognition.end());
		text_result.SetWindowTextW(cvt.c_str());
	}
}
