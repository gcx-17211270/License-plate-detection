// ImgProView.cpp : implementation of the CImgProView class
//

#include "stdafx.h"
#include "ImgPro.h"

#include "ImgProDoc.h"
#include "ImgProView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImgProView

IMPLEMENT_DYNCREATE(CImgProView, CView)

BEGIN_MESSAGE_MAP(CImgProView, CView)
	//{{AFX_MSG_MAP(CImgProView)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImgProView construction/destruction

CImgProView::CImgProView()
{
	// TODO: add construction code here
	image = 0;
	normImg = 0;
	normImg2 = 0;
	findex = 0;
	newWidth=200;
	newHeight=200;
	newWidth2 = 600;
	newHeight2 = 600;
}

CImgProView::~CImgProView()
{
	if( image )
		delete image;
	if( normImg )
		delete normImg;
}

BOOL CImgProView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CImgProView drawing

void CImgProView::OnDraw(CDC* pDC)
{
	CImgProDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here

	if( image == 0 )
		return;

	int i, j;
	BYTE gray;
	for( i=0; i<height; i++)
	{
		for( j=0; j<width; j++)
		{
			gray = image[i*width+j];
			pDC->SetPixel( j, i, RGB(gray,gray,gray));
		}
	}

	//////   show outImg here //////////////////////
	if (normImg)delete normImg;
	normImg = new BYTE[newWidth * newHeight];
	normalize(image, width, height, normImg, newWidth, newHeight);
	normImg2 = new BYTE[newWidth2 * newHeight2];
	normalize(image, width, height, normImg2, newWidth2, newHeight2);


	for (i = 0; i < newHeight; i++)
	{
		for (j = 0; j < newWidth; j++)
		{
			gray = normImg[i * newWidth + j];
			pDC->SetPixel(j+400, i, RGB(gray, gray, gray));
		}
	}
	for (i = 0; i < newHeight2; i++)
	{
		for (j = 0; j < newWidth2; j++)
		{
			gray = normImg2[i * newWidth2 + j];
			pDC->SetPixel(j + 620, i, RGB(gray, gray, gray));
		}
	}
//那么该怎么把图片旋转
}


/////////////////////////////////////////////////////////////////////////////
// CImgProView printing

BOOL CImgProView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CImgProView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CImgProView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CImgProView diagnostics

#ifdef _DEBUG
void CImgProView::AssertValid() const
{
	CView::AssertValid();
}

void CImgProView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImgProDoc* CImgProView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImgProDoc)));
	return (CImgProDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImgProView message handlers

void CImgProView::OnFileOpen() 
{
	// TODO: Add your command handler code here
	CFileDialog MyFDlg(TRUE, NULL,NULL, OFN_HIDEREADONLY, NULL,NULL );

	MyFDlg.m_ofn.lpstrFilter = "(*.lst;*.raw)\0*.lst;*.raw\0";
	MyFDlg.m_ofn.lpstrInitialDir = "C:\\Users\\32353\\Desktop\\bb\\bb";

	if( MyFDlg.DoModal() == IDOK )
	{
		CString SFName;

		SFName = MyFDlg.GetPathName( ); //full name with path

		char *fnstr;
		fnstr = SFName.GetBuffer(4); //read the name from string 

		FILE *fpLst;
		int n;

		int len;
		len = strlen( fnstr );
		if( !strcmp( fnstr+len-3, "raw" ) ) //single raw image
		{
			fnum = 0;
			char *ptr;
			ptr = fnstr+len-3;
			while( *ptr != '\\')
				ptr--;
			*ptr = 0;
			strcpy( directory, fnstr);//Directory

			fnames = new char[500];
			strcpy( fnames, ptr+1); //image name
		}
		else //list file
		{
			fpLst = fopen( fnstr, "rb");
			fscanf( fpLst, "%3d", &fnum);
			fscanf( fpLst, "%s", directory);//directory

			fnames = new char[fnum*100];
			for( n=0; n<fnum; n++)
				fscanf( fpLst, "%s", fnames+n*100);// image names

			fclose(fpLst);
		}

		findex = 0;
		readImg( findex);
	}
}	

void CImgProView::readImg( int findex)
{

	char fullName[120];
	sprintf( fullName, "%s\\%s", directory, fnames+findex*100);

	FILE *fpImg;
	fpImg = fopen( fullName, "rb");
	if( fpImg==0 )
	{
		AfxMessageBox( "Cannot open the image file", MB_OK, 0 );
		return;
	}

	CString sFTitle;
	sFTitle.Format( "%s", fnames+findex*100 );

	CImgProDoc* pDoc;
	pDoc = GetDocument();
	pDoc->SetTitle( sFTitle );

	fread( &width, sizeof(int), 1, fpImg);
	fread( &height, sizeof(int), 1, fpImg);

	if( image )
		delete image;

	image = new BYTE[width*height];

	fread( image, sizeof(BYTE), width*height, fpImg);
	fclose(fpImg);

////////  add processing function here  ///////////////////
	if( normImg )
		delete normImg;
	
	normImg = new BYTE[newWidth*newHeight];
	normImg2 = new BYTE[newWidth2*newHeight2];

	normalize(image,width,height,normImg,newWidth,newHeight);
	


////////////////////////////////////////////////////////////

    OnInitialUpdate();
	CRect ClientRect;
	GetClientRect( &ClientRect );
	InvalidateRect(  &ClientRect );	

}

void CImgProView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if( nChar == VK_NEXT)
	{
		if( findex < fnum )
		{
			findex++;
			readImg( findex );
		}
	}
	if( nChar == VK_PRIOR )
	{
		if( findex > 0 )
		{
			findex--;
			readImg( findex );
		}
	}	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CImgProView::OnFileSave() 
{
	// TODO: Add your command handler code here
	char str[40], *ptr;
	sprintf( str, fnames+findex*40 );
	ptr = str;
	while( *ptr!='.' )
		ptr ++;
	*ptr = 0;

	char fname[80];
	sprintf( fname, "D:\\Teaching\\ImagePro\\bb\\out%s.raw", str );

	FILE* fpOut;
	fpOut = fopen( fname, "wb" );

	fwrite( &width, 4, 1, fpOut );
	fwrite( &height, 4, 1, fpOut );
	fwrite( normImg, 1, newWidth*newHeight, fpOut );

	fclose( fpOut );

}


void CImgProView::normalize(BYTE*image,int width, int height, BYTE* normImg ,int newWid,int newHei)
{
	if (newWid < 2)return;
	if (newHei < 2)return;

	int i, j;
	int row, col;
	for (i = 0; i < newHei; i++)
	{
		row = i * (height - 1) / (newHei - 1);
		for (j = 0; j < newWid; j++) {
			col = j * (width - 1) / (newWid - 1);
			normImg[i*newWid+j]=image[row*width+col];
		}
	}
}