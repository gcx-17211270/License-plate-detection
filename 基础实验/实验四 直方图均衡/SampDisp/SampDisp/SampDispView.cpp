// sampDispView.cpp : implementation of the CSampDispView class
//

#include "stdafx.h"
#include "sampDisp.h"

#include "sampDispDoc.h"
#include "sampDispView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSampDispView

IMPLEMENT_DYNCREATE(CSampDispView, CView)

BEGIN_MESSAGE_MAP(CSampDispView, CView)
	//{{AFX_MSG_MAP(CSampDispView)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampDispView construction/destruction

CSampDispView::CSampDispView()
{
	// TODO: add construction code here
	rowNum = 5;
	colNum = 10;
	pageNum = rowNum*colNum;
	rowStep = 120;
	colStep = 100;

	data = 0;
	eqlFlag = 0;
}

CSampDispView::~CSampDispView()
{
	if (data)
		delete data;
}

BOOL CSampDispView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSampDispView drawing

void CSampDispView::OnDraw(CDC* pDC)
{
	CSampDispDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	if (data == 0)
		return;

	int i, j;
	for (i = 0; i<rowNum; i++)
		for (j = 0; j<colNum; j++)
		{
			if (i*colNum + j>sampNum - 1)
				break;

			drawImg(pDC, data + (i*colNum + j)*dim, j*colStep, i*rowStep, 4);
		}

	if (eqlFlag == 1)// after equalization 
	{
		for (i = 0; i<rowNum; i++)
			for (j = 0; j<colNum; j++)
			{
				if (i*colNum + j>sampNum - 1)
					break;

				drawImg(pDC, data + (i*colNum + j)*dim, j*colStep, i*rowStep, 4);
			}
	}

}

void CSampDispView::drawImg(CDC* pDC, BYTE* array, int xo, int yo, int multi)
{
	int i, j, u, v;
	BYTE gray;
	int winSize = 20;
	int dim = 0;
	for (i = 0; i<winSize; i++)
		for (j = 0; j<winSize; j++)
		{

			gray = array[dim++];
			for (u = 0; u<multi; u++)
				for (v = 0; v<multi; v++)
					pDC->SetPixelV(xo + j*multi + u, yo + i*multi + v, RGB(gray, gray, gray));
		}
}

/////////////////////////////////////////////////////////////////////////////
// CSampDispView printing

BOOL CSampDispView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSampDispView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSampDispView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSampDispView diagnostics

#ifdef _DEBUG
void CSampDispView::AssertValid() const
{
	CView::AssertValid();
}

void CSampDispView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSampDispDoc* CSampDispView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSampDispDoc)));
	return (CSampDispDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSampDispView message handlers

void CSampDispView::OnFileOpen()
{
	// TODO: Add your command handler code here
	CFileDialog MyFDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, NULL, NULL);

	MyFDlg.m_ofn.lpstrFilter = "List Files (*.dat;*.eql)\0*.dat;*.eql\0";
	if (data == 0)
		MyFDlg.m_ofn.lpstrInitialDir = "D:";

	if (MyFDlg.DoModal() == IDOK)
	{
		CString sFName;
		char* fnstr;
		FILE* fpDat;

		sFName = MyFDlg.GetPathName();	// full name including the path
		fnstr = sFName.GetBuffer(4);

		fpDat = fopen(fnstr, "rb");
		if (fpDat == 0)
		{
			AfxMessageBox("Cannot open the data file", MB_OK, 0);
			return;
		}

		fread(&sampNum, 4, 1, fpDat);
		fread(&dim, 4, 1, fpDat);

		if (data)
			delete data;

		data = new BYTE[sampNum*dim];
		fread(data, 1, sampNum*dim, fpDat);
		fclose(fpDat);

		FILE *fpOut;
		fpOut = fopen("train20.eql", "wb");
		fwrite(&sampNum, sizeof(int), 1, fpOut);
		fwrite(&dim, sizeof(int), 1, fpOut);

		BYTE *temp;
		temp = new BYTE[dim];
		int n;
		for (n = 0; n<sampNum; n++)
		{
			hisEqualiz(data + n*dim, 20, 20, temp);
			fwrite(temp, 1, dim, fpOut);
		}
		fclose(fpOut);

		delete temp;

		CRect rectClient;
		GetClientRect(&rectClient);
		InvalidateRect(&rectClient, 1);

	}
}

void CSampDispView::hisEqualiz(BYTE*image, int w, int h, BYTE*outImg)
{
	int hist[256];
	int n, m, i, j;

	//initialize
	for (n = 0; n < 256; n++)hist[n] = 0;

	//compute
	for (i = 0; i < h; i++) {
		for (j = 0; j < 2; j++) {
			hist[image[i * w + j]]++;
		}
	}

	for (n = 1; n < 256; n++) {
		hist[n] += hist[n - 1];
	}

	//compute the new graylevel
	BYTE gray[256];
	float cons;
	cons = 255. / hist[255];

	for (n = 0; n < 256; n++)
		gray[n] = (BYTE)(cons * hist[n]);

	for (i = 0; i < h; i++)
		for (j = 0; j < w; j++)
			outImg[i * w + j] = gray[image[i * w + j]];

}

void CSampDispView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	eqlFlag = 1;

	FILE *fpOut;
	fpOut = fopen("train20.eql", "rb");
	fread(&sampNum, sizeof(int), 1, fpOut);
	fread(&dim, sizeof(int), 1, fpOut);
	fread(data, 1, sampNum*dim, fpOut);

	CRect rectClient;
	GetClientRect(&rectClient);
	InvalidateRect(&rectClient, 1);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}