// ImgProView.cpp : implementation of the CImgProView class
//

#include <stdafx.h>
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
	findex = 0;
	newWidth = 480;
	newHeight = 480;

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
	for (i = 0; i < newHeight; i++)
	{
		for (j = 0; j < newWidth; j++)
		{
			gray = normImg[i * newWidth + j];
			pDC->SetPixel(j+width+10, i, RGB(gray, gray, gray));
		}
	}
	for (i = 0; i < DstHeight; i++)
	{
		for (j = 0; j < DstWidth; j++)
		{
			gray = rotationImg[i * DstWidth + j];
			pDC->SetPixel(j + newWidth + 10 +width + 10, i, RGB(gray, gray, gray));
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
	MyFDlg.m_ofn.lpstrInitialDir = "C:\\Users\\32353\\Desktop\\bb";

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
   if (normImg)
		delete normImg;

   normImg = new BYTE[newWidth*newHeight];
   
   normalize_Bilinear_interpolation(image, width, height, normImg,newWidth, newHeight);
   rotate(image, width, height, (float)45);

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
	fwrite( normImg, 1, width*height, fpOut );

	fclose( fpOut );

}
void CImgProView::normalize_Bilinear_interpolation(BYTE* image, int width, int height, BYTE*normImg, int newWid, int newHei)
{
	if (newWid < 2)return;
	if (newHei < 2)return;
	double xRatio = (double)width * 1.0 / newWid;//横向比
	double yRatio = (double)height * 1.0 / newHei;//纵向比
	int i, j;
	for (i = 0; i < newHei; i++)
	{
		double srcY = i * yRatio;		//源图像“虚”坐标的y值
		int IntY = (int)srcY;			//向下取整
		double v = srcY - IntY;			//获取小数部分
		double v1 = 1.0 - v;

		for (j = 0; j < newWid; j++) {
			double srcX = j * xRatio;	//源图像“虚”坐标的x值
			int IntX = (int)srcX;		//向下取整
			double u = srcX - IntX;		//获得小数部分
			double u1 = 1.0 - u;
			int Index00 = IntY * width + IntX;	//得到原图左上角的像素位置

			int Index10;						//原图左下脚的像素位置
			if (IntY < height - 1) {
				Index10 = Index00 + width;
			}
			else Index10 = Index00;

			int Index01;				//原图右上角的像素位置
			int Index11;				//原图右下角的像素位置
			if (IntX < width - 1) {
				Index01 = Index00 + 1;
				Index11 = Index10 + 1;
			}
			else {
				Index01 = Index00;
				Index11 = Index10;
			}
			double temp = (v1 * (u * image[Index01] + u1 * image[Index00]) + v * (u * image[Index11] + u1 * image[Index10]));
			normImg[i * newWid + j] = (BYTE)temp;
		}
	}
}

void CImgProView::rotate(BYTE* image, int width, int height, float ang)
{
	float PI = 3.1415926;
	float rad = ang * (PI / 180);
	//四周的点左边转换
	int SrcX1 = -width / 2;
	int SrcY1 = height / 2;
	int SrcX2 = width / 2;
	int SrcY2 = height / 2;
	int SrcX3 = width / 2;
	int SrcY3 = -height / 2;
	int SrcX4 = -width / 2;
	int SrcY4 = -height / 2;

	int DstX1 = (int)((cos(rad) * SrcX1 + sin(rad) * SrcY1) + 0.5);
	int DstY1 = (int)((-sin(rad) * SrcX1 + cos(rad) * SrcY1) + 0.5);
	int DstX2 = (int)((cos(rad) * SrcX2 + sin(rad) * SrcY2) + 0.5);
	int DstY2 = (int)((-sin(rad) * SrcX2 + cos(rad) * SrcY2) + 0.5);
	int DstX3 = (int)((cos(rad) * SrcX3 + sin(rad) * SrcY3) + 0.5);
	int DstY3 = (int)((-sin(rad) * SrcX3 + cos(rad) * SrcY3) + 0.5);
	int DstX4 = (int)((cos(rad) * SrcX4 + sin(rad) * SrcY4) + 0.5);
	int DstY4 = (int)((-sin(rad) * SrcX4 + cos(rad) * SrcY4) + 0.5);

	//求出旋转后的图片大小，可以都置成白色
	DstWidth = max(abs(DstX1 - DstX3), abs(DstX2 - DstX4)) + 1;
	DstHeight = max(abs(DstY1 - DstY3), abs(DstY2 - DstY4)) + 1;
	rotationImg = new BYTE[DstHeight * DstWidth];

	for(int i=0;i<DstHeight;i++)			//i,j为现在的图的坐标
		for (int j = 0; j < DstWidth; j++)
		{
			int x = (int)(cos(rad) * (j - DstWidth / 2) + sin(rad) * (DstHeight/2 - i));//X,Y是原图中的像素坐标
			int y = (int)(-sin(rad) * (j - DstWidth / 2) + cos(rad) * (DstHeight / 2 - i));

			x = x + height / 2;
			y = width / 2 - y;

			if (x > (width - 1) || x<0 || y>(height - 1) || y < 0)
			{
				rotationImg[i * DstWidth + j] = 255;
			}
			else
			{
				rotationImg[i * DstWidth + j] = image[y * width + x];
			}
		}
}