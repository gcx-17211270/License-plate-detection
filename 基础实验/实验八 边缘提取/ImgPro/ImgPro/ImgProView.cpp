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
	filtImg = 0;
	findex = 0;

}

CImgProView::~CImgProView()
{
	if( image )
		delete image;
	if( filtImg )
		delete filtImg;
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

	for( i=0; i<height; i++)
	{
		for( j=0; j<width; j++)
		{
			gray = filtImg[i*width+j];
			pDC->SetPixel( width+10+j, i, RGB(gray,gray,gray));
		}
	}

	
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
	
	if( filtImg )
		delete filtImg;

	filtImg = new BYTE[width*height];

	int *tempX, *tempY;
	 tempX = new int[width*height];
	 tempY = new int[width*height];

	sobel(image, width,height,tempX,tempY);
	tranByte(tempX,filtImg,width,height);

	delete tempX;
	delete tempY;

	
////////////////////////////////////////////////////////////

    OnInitialUpdate();
	CRect ClientRect;
	GetClientRect( &ClientRect );
	InvalidateRect(  &ClientRect );	

}

void CImgProView::sobel( BYTE*window, int wid, int hei, int *sob_x,int *sob_y)
						            
{
	int so_x[9]; //horizontal 
	so_x[0] = -1;
	so_x[1] = 0;
	so_x[2] = 1;
	so_x[3] = -2;
	so_x[4] = 0;
	so_x[5] = 2;
	so_x[6] = -1;
	so_x[7] = 0;
	so_x[8] = 1;

	int so_y[9]; //vertical
	so_y[0] = -1;
	so_y[1] = -2;
	so_y[2] = -1;
	so_y[3] = 0;
	so_y[4] = 0;
	so_y[5] = 0;
	so_y[6] = 1;
	so_y[7] = 2;
	so_y[8] = 1;

	int i, j, m,n;
	BYTE block[9];

	int value;
	for(i=0;i<hei;i++)
		for (j = 0; j < wid; j++) {
			if (i == 0 || j == 0 || i == wid - 1 || j == hei - 1) {
				sob_x[i * wid + j] = 0;
				sob_y[i * wid + j] = 0;
			}
			else {
				for (m = -1; m < 2; m++)
					for (n = -1; n < 2; n++)
						block[(m + 1) * 3 + n + 1] = window[(i + m) * wid + j + n];
				value = convolution(so_x, block);
				sob_x[i * wid + j] = value;

				value = convolution(so_y, block);
				sob_y[i * wid + j] = value;
			}
		}
	
}

int CImgProView::convolution( int *operatr, BYTE*block)
{
	int value;

	int i, j;
	value = 0;
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			value +=operatr[i*3+j]*block[i*3+j];
	
	return value;
}

void CImgProView::tranByte( int*temp, BYTE* array, int w, int h)
{
	int dim;
	dim = w*h;

	double stm,stvar;
	stm = 120;
	stvar = 60;
	int i;

	double mean = 0;
	for( i=0; i<dim; i++ )
			mean += temp[i];
	mean /= dim; // mean value
		
	double varia = 0;
	double diff;
	for( i=0; i<dim; i++ )
	{
			diff = temp[i]-mean;
			varia += diff*diff;
	}
	varia /= dim; // varianve

	if( varia<0.0001 )
		return;

	double ratio;
	ratio = stvar/sqrt( varia );

	BYTE value;
	for( i=0; i<dim; i++ )
	{
		value = (BYTE)( (temp[i]-mean)*ratio+stm );

		if( value >= 200 )
			array[i] = 200;
		else if( value < 80 )
			array[i] = 80;
		else
			array[i] = value;
		
	}
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
	fwrite( filtImg, 1, width*height, fpOut );

	fclose( fpOut );

}
