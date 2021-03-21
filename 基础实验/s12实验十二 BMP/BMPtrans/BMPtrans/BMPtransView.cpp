// BMPtransView.cpp : implementation of the CBMPtransView class
//

#include "stdafx.h"
#include "BMPtrans.h"

#include "BMPtransDoc.h"
#include "BMPtransView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBMPtransView

IMPLEMENT_DYNCREATE(CBMPtransView, CView)

BEGIN_MESSAGE_MAP(CBMPtransView, CView)
	//{{AFX_MSG_MAP(CBMPtransView)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBMPtransView construction/destruction

CBMPtransView::CBMPtransView()
{
	// TODO: add construction code here
	image = 0;
	rgbImg = 0;
	bmpflag = 0;
}
 

CBMPtransView::~CBMPtransView()
{
	if( image )
		delete image;
	if( rgbImg )
		delete rgbImg;
}

BOOL CBMPtransView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CBMPtransView drawing

void CBMPtransView::OnDraw(CDC* pDC)
{
	CBMPtransDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here

	if( image==0 )
		return;

	int i, j;
	BYTE gray;
	for( i=0; i<height; i++ )
		for( j=0; j<width; j++ )
		{
			gray = image[i*width+j];
			pDC->SetPixelV( j, i, RGB(gray, gray, gray) );
		}
	pDC->TextOutA(0, 0, "RAW FILE");

	BYTE r,g,b;
	if(bmpflag == 1)
	{
		for( i=0; i<height; i++ )
			for( j=0; j<3*width; j=j+3 )
			{
				b = rgbImg[i*3*width+j];
				g = rgbImg[i*3*width+j+1];
				r = rgbImg[i*3*width+j+2];
				pDC->SetPixelV( width+10+j/3, i, RGB(r, g, b) );
			}
	pDC->TextOutA(width+10, 0, "BMP FILE");
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBMPtransView printing

BOOL CBMPtransView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CBMPtransView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CBMPtransView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CBMPtransView diagnostics

#ifdef _DEBUG
void CBMPtransView::AssertValid() const
{
	CView::AssertValid();
}

void CBMPtransView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBMPtransDoc* CBMPtransView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBMPtransDoc)));
	return (CBMPtransDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBMPtransView message handlers

void CBMPtransView::OnFileOpen() 
{
	// TODO: Add your command handler code here
	CFileDialog MyFDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, NULL, NULL);

	MyFDlg.m_ofn.lpstrFilter = "List Files (*.bmp;*.raw)\0*.bmp;*.raw\0";
	if( image==0 )
		MyFDlg.m_ofn.lpstrInitialDir = "C:\\Users\a1176\Desktop\E12";

	if( MyFDlg.DoModal()==IDCANCEL )
		return;

	CString sFName;
	sFName = MyFDlg.GetPathName();	// full name including the path

	char* fnstr;
	fnstr = sFName.GetBuffer( 4 );
	strcpy( imgFName, fnstr );

	FILE* fp;
	fp = fopen( fnstr, "rb" );
	if( fp==0 )
	{
		AfxMessageBox( "Cannot open the image file", MB_OK, 0 );
		return;
	}

	if( image )
		delete image;
	
	CBMPtransDoc* pDoc;
	pDoc = GetDocument();
	pDoc->SetTitle( sFName );

	// raw file
	if( !strcmp( fnstr+strlen(fnstr)-3, "raw" ) )
	{
		fread( &width, 4, 1, fp );
		fread( &height, 4, 1, fp );

		image = new BYTE [width*height];
		fread( image, 1, width*height, fp );
	}
	// bmp file
	else 
	{
		bmpflag = 1;
		int bicount;

		BITMAPFILEHEADER bmpFHeader;
		BITMAPINFOHEADER bmiHeader;

		fread(&bmpFHeader, sizeof(BITMAPFILEHEADER), 1, fp);
		fread(&bmiHeader, sizeof(BITMAPINFOHEADER), 1, fp);
		width = bmiHeader.biWidth;
		height = bmiHeader.biHeight;
		bicount = bmiHeader.biBitCount;

		int rowLen = ((width * bicount) / 8 + 3) / 4 * 4;
		BYTE* rowBuff = new BYTE[rowLen];

		if (bicount == 8) {
			RGBQUAD bmiColors[256];
			fread(bmiColors, sizeof(RGBQUAD), 256, fp);
			image = new BYTE[width * height];
			rgbImg = new BYTE[3 * width * height];
			for (int i = height - 1; i >= 0; --i) {
				fread(rowBuff, 1, rowLen, fp);
				memcpy(image + i * width, rowBuff, width);
			}
			for (int i = 0; i < height; ++i)
				for (int j = 0; j < 3 * width; j += 3) {
					rgbImg[i * 3 * width + j] = bmiColors[image[i * width + j / 3]].rgbBlue;
					rgbImg[i * 3 * width + j + 1] = bmiColors[image[i * width + j / 3]].rgbGreen;
					rgbImg[i * 3 * width + j + 2] = bmiColors[image[i * width + j / 3]].rgbRed;
				}
		} else if (bicount == 24) {
			image = new BYTE[width * height];
			rgbImg = new BYTE[rowLen * height];
			for (int i = height - 1; i >= 0; i--)
			{
				fread(rowBuff, 1, rowLen, fp);
				memcpy(rgbImg + i * 3 * width, rowBuff, 3 * width);
			}
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width; ++j) {
					image[i * width + j] = rgbImg[3 * (i * width + j)];
				}
			}
		}
		delete[] rowBuff;

	}
	fclose(fp);

	CRect rectClient;
	GetClientRect( &rectClient );
	InvalidateRect( &rectClient, TRUE );
}

// If the loaded image is BMP, saved in RAW
// else if the loaded image is RAW, saved in BMP
void CBMPtransView::OnFileSave() 
{
	if( image==0 )
		return;

	// TODO: Add your command handler code here
	FILE* fpOut;
	char fname[80];
	int len;
	len = strlen( imgFName );

	int BMP;
	strcpy( fname, imgFName );
	if( !strcmp( imgFName+len-3, "raw" ) )
	{
		strcpy( fname+len-3, "bmp" );
		BMP = 1;
	}
	else
	{
		strcpy( fname+len-3, "raw" );
		BMP = 0;
	}

	fpOut = fopen( fname, "wb" );

	if( BMP==0 )//Save into raw format
	{
		width=198;
		height=198;
		fwrite( &width, 4, 1, fpOut );
		fwrite( &height, 4, 1, fpOut );
		fwrite( image, 1, width*height, fpOut );
	}
	else
	{
		int rowLen;
		BYTE* rowBuff;

		rowLen = ( (width+3)/4 )*4;
		rowBuff = new BYTE [rowLen];

		BITMAPFILEHEADER bmpFHeader;
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD bmiColors[256];

		for( int g=0; g<256; g++ )
		{
			bmiColors[g].rgbBlue = g;
			bmiColors[g].rgbRed = g;
			bmiColors[g].rgbGreen = g;
			bmiColors[g].rgbReserved = 0;
		}

		bmpFHeader.bfType = 'B'+256*'M';
		bmpFHeader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)
			+256*sizeof(RGBQUAD);
		bmpFHeader.bfSize = width*height+bmpFHeader.bfOffBits;
		bmpFHeader.bfReserved1 = 0;
		bmpFHeader.bfReserved2 = 0;

		bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmiHeader.biWidth = width;
		bmiHeader.biHeight = height;
		bmiHeader.biPlanes = 1;
		bmiHeader.biBitCount = 8;
		bmiHeader.biCompression = 0;
		bmiHeader.biSizeImage = 0;
		bmiHeader.biXPelsPerMeter = 0;
		bmiHeader.biYPelsPerMeter = 0;
		bmiHeader.biClrUsed = 0;
		bmiHeader.biClrImportant = 0;

		fwrite( &bmpFHeader, sizeof(BITMAPFILEHEADER), 1, fpOut );
		fwrite( &bmiHeader, sizeof(BITMAPINFOHEADER), 1, fpOut );
		fwrite( bmiColors, sizeof(RGBQUAD), 256, fpOut );

		for( int i=height-1; i>=0; i-- )
		{
			memcpy( rowBuff, image+i*width, width );
			fwrite( rowBuff, 1, rowLen, fpOut );
		}

		delete rowBuff;
	}

	fclose( fpOut );
}

void CBMPtransView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	
}

