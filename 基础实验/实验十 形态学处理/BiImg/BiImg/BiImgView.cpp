// BiImgView.cpp : implementation of the CBiImgView class
//

#include "stdafx.h"
#include "BiImg.h"

#include "BiImgDoc.h"
#include "BiImgView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBiImgView

IMPLEMENT_DYNCREATE(CBiImgView, CView)

BEGIN_MESSAGE_MAP(CBiImgView, CView)
	//{{AFX_MSG_MAP(CBiImgView)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBiImgView construction/destruction

CBiImgView::CBiImgView()
{
	// TODO: add construction code here
	image = 0;
	outImg1 = 0;
	outImg2 = 0;

	findex = 0;

    LButtonDown = 0;
	RButtonDown = 0;
}

CBiImgView::~CBiImgView()
{
	if( image )
		delete image;
	if( outImg1 )
		delete outImg1;
	if( outImg2 )
		delete outImg2;
}

BOOL CBiImgView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CBiImgView drawing

void CBiImgView::OnDraw(CDC* pDC)
{
	CBiImgDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here

	if( image == 0 )
		return;

	int i, j;
	BYTE gray;
	BYTE g;
	for( i=0; i<height; i++)
	{
		for( j=0; j<width; j++)
		{
			gray = image[i*width+j];
			pDC->SetPixel( j, i, RGB(gray,gray,gray));
		}
	}

	if( RButtonDown ) // erosion and dilation
	{
		for( i=0; i<height; i++)
		{
			for( j=0; j<width; j++)
			{
				g = outImg1[i*width+j];
				pDC->SetPixel( j, i+height+10, RGB(g,g,g));
	    		pDC->TextOut(10, height+10, "erosion");

				g = outImg2[i*width+j];
				pDC->SetPixel( j+width+10, i+height+10, RGB(g,g,g));
	    		pDC->TextOut(width+10, height+10, "dilation");
			}
		}

	}
	
	if( LButtonDown ) //closing and opening
	{
		for( i=0; i<height; i++)
	       {
			 for( j=0; j<width; j++)
			 {
				g = outImg1[i*width+j];
				pDC->SetPixel( j, i+height+10, RGB(g,g,g));
	    		pDC->TextOut(10, height+10, "opening");

				g = outImg2[i*width+j];
				pDC->SetPixel( j+width+10, i+height+10, RGB(g,g,g));
	    		pDC->TextOut(width+10, height+10, "closing");
			 }			
		    }
	}

	LButtonDown = 0;
	RButtonDown = 0;
	
}



/////////////////////////////////////////////////////////////////////////////
// CBiImgView printing

BOOL CBiImgView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CBiImgView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CBiImgView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CBiImgView diagnostics

#ifdef _DEBUG
void CBiImgView::AssertValid() const
{
	CView::AssertValid();
}

void CBiImgView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBiImgDoc* CBiImgView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBiImgDoc)));
	return (CBiImgDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBiImgView message handlers

void CBiImgView::OnFileOpen() 
{
	// TODO: Add your command handler code here
	CFileDialog MyFDlg(TRUE, NULL,NULL, OFN_HIDEREADONLY, NULL,NULL );

	MyFDlg.m_ofn.lpstrFilter = "(*.lst;*.raw)\0*.lst;*.raw\0";
	MyFDlg.m_ofn.lpstrInitialDir = "C:\\Users\\32353\\Desktop\\实验十 形态学处理";

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
void CBiImgView::readImg( int findex )
{
	char fullName[120];
	sprintf( fullName, "%s\\%s", directory, fnames+findex*100);

	FILE *fpImg;
	fpImg = fopen( fullName, "rb");
	if( fpImg==0 )
	{
		AfxMessageBox( "Cannot open the list file", MB_OK, 0 );
		return;
	}

	CString sFTitle;
	sFTitle.Format( "%s", fnames+findex*100 );

	CBiImgDoc* pDoc;
	pDoc = GetDocument();
	pDoc->SetTitle( sFTitle );

	fread( &width, sizeof(int), 1, fpImg);
	fread( &height, sizeof(int), 1, fpImg);

	if( image )
		delete image;

	image = new BYTE[width*height];

	fread( image, sizeof(BYTE), width*height, fpImg);
	fclose(fpImg);

    OnInitialUpdate();
	CRect ClientRect;
	GetClientRect( &ClientRect );
	InvalidateRect(  &ClientRect );	
}

void CBiImgView::erosion(BYTE *image, int w, int h, BYTE*outImg)
{
	int rept;
	memcpy(outImg, image, sizeof(BYTE) * width * height);

	int i, j;
	int m, n;
	BYTE flag;
	for (rept = 0; rept < 1; rept++) {
		for (i = 1; i < h - 1; i++) {
			for (j = 1; j < w - 1; j++) {
				if (image[i * w + j] == 255) {
					flag = 0;
					for (m = -1; m < 2; m++) {
						for (n = -1; n < 2; n++)
						{
							if (image[(i + m) * w + j + n] == 0)
							{
								flag++;
								break;
							}
						}
					}
					if(flag > 0)outImg[i * w + j] = 0;
				}
			}
		}
		memcpy(image, outImg, sizeof(BYTE) * width * height);
	}
	
}

void CBiImgView::dilation(BYTE *image, int w, int h, BYTE*outImg)
{
	int rept;
	memcpy(outImg, image, sizeof(BYTE) * width * height);

	int i, j;
	int m, n;
	BYTE flag;

	for (rept = 0; rept < 1; rept++)
	{
		for (i = 1; i < h - 1; i++)
		{
			for (j = 1; j < w - 1; j++)
			{
				if (image[i * w + j] == 0)
				{
					flag = 0;
					for (m = -1; m < 2; m++)
					{
						for (n = -1; n < 2; n++)
						{
							if (image[(i + m) * w + j] == 255)
								flag++;
						}
					}
					if (flag > 0)outImg[i * w + j] = 255;
				}
			}
		}
		memcpy(image, outImg, sizeof(BYTE) * width * height);
	}
}


void CBiImgView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	LButtonDown = 1;

	if (outImg1)delete outImg1;
	outImg1 = new BYTE[width * height];

	if (outImg2)delete outImg2;
	outImg2 = new BYTE[width * height];

	BYTE* temp;
	temp = new BYTE[width * height];

	BYTE* tempOut;
	tempOut = new BYTE[width * height];

	memcpy(temp, image, sizeof(BYTE) * width * height);

	//openint save into outImg1
	int n;
	for (n = 0; n < 2; n++)
	{
		erosion(temp, width, height, tempOut);
		dilation(tempOut, width, height, temp);
	}
	memcpy(outImg1, temp, sizeof(BYTE) * width * height);

	//closing save into outImg2
	memcpy(temp, image, sizeof(BYTE) * width * height);
	dilation(temp, width, height, tempOut);
	erosion(tempOut, width, height, outImg2);

	delete temp;
	delete tempOut;

	OnInitialUpdate();
	CRect ClientRect;
	GetClientRect(&ClientRect);
	InvalidateRect(&ClientRect);

	CView::OnLButtonDown(nFlags, point);
}

void CBiImgView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	RButtonDown = 1;

	if( outImg1 )
		delete outImg1;
	outImg1 = new BYTE[width*height];

 	if( outImg2 )
		delete outImg2;
	outImg2 = new BYTE[width*height];

	BYTE *temp;
	temp = new BYTE[width*height];

	//erosion save into outImg1	
  	memcpy( temp, image, sizeof(BYTE)*width*height);
	erosion( temp, width, height,  outImg1); 	

	//dilation   save into outImg2
 	memcpy( temp, image, sizeof(BYTE)*width*height);
	dilation(temp, width, height,outImg2 );

	delete temp;

    OnInitialUpdate();
	CRect ClientRect;
	GetClientRect( &ClientRect );
	InvalidateRect(  &ClientRect );	

	CView::OnRButtonDown(nFlags, point);
}
