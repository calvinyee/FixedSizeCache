
// PDFPageCacheDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PDFPageCacheDemo.h"
#include "PDFPageCacheDemoDlg.h"
#include "afxdialogex.h"
#include "CacheTable.h"

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


// CPDFPageCacheDemoDlg dialog



CPDFPageCacheDemoDlg::CPDFPageCacheDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PDFPAGECACHEDEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPDFPageCacheDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPDFPageCacheDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CPDFPageCacheDemoDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CPDFPageCacheDemoDlg message handlers

BOOL CPDFPageCacheDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPDFPageCacheDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPDFPageCacheDemoDlg::OnPaint()
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
HCURSOR CPDFPageCacheDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPDFPageCacheDemoDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	char* pCharData = "This is a test";
	int nDataSize = (strlen(pCharData) + 1) * sizeof(char);

	CacheTable cacheTable(13, true);


	// add
	cacheTable.AddAndUpdateCacheData(0, (void *)pCharData, nDataSize);
	cacheTable.AddAndUpdateCacheData(1, (void *)pCharData, nDataSize);
	cacheTable.AddAndUpdateCacheData(2, (void *)pCharData, nDataSize);
	cacheTable.AddAndUpdateCacheData(3, (void *)pCharData, nDataSize);
	cacheTable.AddAndUpdateCacheData(4, (void *)pCharData, nDataSize);
	cacheTable.AddAndUpdateCacheData(5, (void *)pCharData, nDataSize);
	cacheTable.AddAndUpdateCacheData(6, (void *)pCharData, nDataSize);
	cacheTable.AddAndUpdateCacheData(7, (void *)pCharData, nDataSize);
	cacheTable.AddAndUpdateCacheData(8, (void *)pCharData, nDataSize);
	cacheTable.AddAndUpdateCacheData(9, (void *)pCharData, nDataSize);
	cacheTable.AddAndUpdateCacheData(10, (void *)pCharData, nDataSize);
	cacheTable.AddAndUpdateCacheData(11, (void *)pCharData, nDataSize);
	cacheTable.AddAndUpdateCacheData(12, (void *)pCharData, nDataSize);

	// get
	CacheTableEntry* pEntry = NULL;

	cacheTable.GetCacheData(0, pEntry);
	cacheTable.GetCacheData(1, pEntry);
	cacheTable.GetCacheData(2, pEntry);
	cacheTable.GetCacheData(3, pEntry);
	cacheTable.GetCacheData(4, pEntry);
	cacheTable.GetCacheData(5, pEntry);
	cacheTable.GetCacheData(6, pEntry);
	cacheTable.GetCacheData(7, pEntry);
	cacheTable.GetCacheData(8, pEntry);
	cacheTable.GetCacheData(9, pEntry);

	cacheTable.GetCacheData(3, pEntry);
	cacheTable.GetCacheData(3, pEntry);

	cacheTable.DeleteCacheData(12);

	int nCount = cacheTable.GetCacheCount();

	int key = -1;
	cacheTable.GetKeyWithMostHitCount(key);

	key = -1;
	cacheTable.GetKeyWithLeastHitCount(key);

	bool bExist;
	cacheTable.IsCacheDataExist(12, bExist);
	cacheTable.IsCacheDataExist(11, bExist);


	char* pStr = (char *)pEntry->GetData();


	cacheTable.ResizeTable(6);
	cacheTable.GetCacheData(3, pEntry);

	if (pEntry != NULL)
	{
		pStr = (char *)pEntry->GetData();
	}



}
