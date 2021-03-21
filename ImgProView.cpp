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
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_32771, &CImgProView::Color)
	ON_COMMAND(ID_32772, &CImgProView::Imgsegmentation)
	ON_COMMAND(ID_32773, &CImgProView::imgdivided)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImgProView construction/destruction

CImgProView::CImgProView()
{
	// TODO: add construction code here
	image = 0;
	outImg = 0;
	findex = 0;

}

CImgProView::~CImgProView()
{
	if( image )
		delete image;
	if( outImg )
		delete outImg;
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
	if(rawflag == 1)
	{
		BYTE gray;
		for( i=0; i<height; i++)
		{
			for( j=0; j<width; j++)
			{
				gray = image[i*width+j];
				pDC->SetPixel( j, i, RGB(gray,gray,gray));
			}
		}
	}
	//////   show outImg here //////////////////////

	if(bmpflag == 1)
	{
		pDC->TextOut(0, 0, "车牌识别：原始图像");
		BYTE r,g,b;
		for( i=0; i<height; i++ )
			for( j=0; j<3*width; j=j+3 )
			{
				b = rgbimg[i*3*width+j];
				g = rgbimg[i*3*width+j+1];
				r = rgbimg[i*3*width+j+2];
				pDC->SetPixelV( j/3, i+30, RGB(r, g, b) );
			}
	}
	if(flag == 1)
	{
		BYTE r, g, b;
		BYTE gray;
		if (0)				//设置为1可看出在图像处理过程中的各个步骤，但容易导致程序卡死
		{
			if (huiimg2)
			{
				pDC->TextOut(width + 10, 0, "进行颜色识别，判断出符合车牌颜色的元素");
				for (i = 0; i < height; i++)
				{
					for (j = 0; j < width; j++)
					{
						gray = huiimg2[i * width + j];
						pDC->SetPixel(j + width + 10, i + 30, RGB(gray, gray, gray));
					}
				}
			}
			if (huiimg3)
			{
				pDC->TextOutA(width * 2 + 10 * 2, 0, "腐蚀与膨胀，准备判断车牌范围");
				for (i = 0; i < height; i++)
				{
					for (j = 0; j < width; j++)
					{
						gray = huiimg3[i * width + j];
						pDC->SetPixel(j + width * 2 + 10 * 2, i + 30, RGB(gray, gray, gray));
					}
				}
			}
			if (huiimg)
			{
				pDC->TextOutA(width * 3 + 10 * 3, 0, "判断车牌范围");
				for (i = 0; i < height; i++)
				{
					for (j = 0; j < width; j++)
					{
						gray = huiimg[i * width + j];
						pDC->SetPixel(j + width * 3 + 10 * 3, i + 30, RGB(gray, gray, gray));
					}
				}
			}
		}
		else if (huiimg)
		{
			pDC->TextOutA(width + 10, 0, "颜色分割：判断车牌范围");
			for (i = 0; i < height; i++)
			{
				for (j = 0; j < width; j++)
				{
					gray = huiimg[i * width + j];
					//pDC->SetPixel(j + width * 3 + 10 * 3, i + 30, RGB(gray, gray, gray));
					pDC->SetPixel(j + width + 10, i + 30, RGB(gray, gray, gray));
				}
			}
		}
	}

	if (segflag == 1)
	{
		BYTE gray;
		BYTE r, g, b;
		if (flag == 0)
			pDC->TextOutA(width + 10, 0, "车牌提取：分割出车牌");
		else;
		for (i = 0; i < newheight; i++)
		{
			for (j = 0; j < 3 * newwidth; j = j + 3)
			{
				b = newrgbimg[i * 3 * newwidth + j];
				g = newrgbimg[i * 3 * newwidth + j + 1];
				r = newrgbimg[i * 3 * newwidth + j + 2];
				pDC->SetPixelV(j / 3 + width + 10, i + 30, RGB(r, g, b));
			}
		}
/*		if (newhuiimg)
		{
			pDC->TextOutA(width + newwidth + 30, 10, "车牌图像灰度图片");
			for (i = 0; i < newheight; i++)
			{
				for (j = 0; j < newwidth; j++)
				{
					gray = newhuiimg[i * newheight + j];
					pDC->SetPixel(j + width + newwidth + 30, i + 30, RGB(gray, gray, gray));
				}
			}
		}
		else pDC->TextOutA(width + newwidth + 30, 10, "打开图片失败");
*/
	}
	if (divflag == 1)
	{
		pDC->TextOutA(width + 10, 0, "扫描分割：");
		pDC->MoveTo(width + 10 + newwidth / 440.0 * 66.5, 30);
		pDC->LineTo(width + 10 + newwidth / 440.0 * 66.5, 30 + newheight);
		pDC->MoveTo(width + 10 + newwidth / 440.0 * 123.5, 30);
		pDC->LineTo(width + 10 + newwidth / 440.0 * 123.5, 30 + newheight);
		pDC->MoveTo(width + 10 + newwidth / 440.0 * 145.5, 30);
		pDC->LineTo(width + 10 + newwidth / 440.0 * 145.5, 30 + newheight);
		pDC->MoveTo(width + 10 + newwidth / 440.0 * 202.5, 30);
		pDC->LineTo(width + 10 + newwidth / 440.0 * 202.5, 30 + newheight);
		pDC->MoveTo(width + 10 + newwidth / 440.0 * 259.5, 30);
		pDC->LineTo(width + 10 + newwidth / 440.0 * 259.5, 30 + newheight);
		pDC->MoveTo(width + 10 + newwidth / 440.0 * 316.5, 30);
		pDC->LineTo(width + 10 + newwidth / 440.0 * 316.5, 30 + newheight);
		pDC->MoveTo(width + 10 + newwidth / 440.0 * 373.5, 30);
		pDC->LineTo(width + 10 + newwidth / 440.0 * 373.5, 30 + newheight);
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

	MyFDlg.m_ofn.lpstrFilter = "(*.lst;*.raw;*.bmp)\0*.lst;*.raw;*.bmp\0";
	MyFDlg.m_ofn.lpstrInitialDir = "C:\\Users\\32353\\Desktop\\点击刷新\\研究性作业2020\\图像";

	flag=0;
	bmpflag = 0;
	rawflag = 0;
//*************************************************************
//2019.12.21,刘皓郡，车牌分割和车牌扫描分割的标志位
	segflag = 0;
	divflag = 0;
//*************************************************************
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
			rawflag = 1;
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
		if( !strcmp( fnstr+len-3, "lst" ) ) //list file
		{
			bmpflag = 1;
			fpLst = fopen( fnstr, "rb");
			fscanf( fpLst, "%3d", &fnum);
			fscanf( fpLst, "%s", directory);//directory

			fnames = new char[fnum*100];
			for( n=0; n<fnum; n++)
				fscanf( fpLst, "%s", fnames+n*100);// image names

			fclose(fpLst);
		}
		if( !strcmp( fnstr+len-3, "bmp" ) )
		{
			bmpflag = 1;
			fnum = 0;
			char *ptr;
			ptr = fnstr+len-3;
			while( *ptr != '\\')
				ptr--;
			*ptr = 0;
			strcpy( directory, fnstr);//Directory

			fnames = new char[500];
			strcpy( fnames, ptr+1); //image name

			findex = 0;
		}
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
		AfxMessageBox( "Cannot open the list file", MB_OK, 0 );
		return;
	}

	CString sFTitle;
	sFTitle.Format( "%s", fnames+findex*100 );

	CImgProDoc* pDoc;
	pDoc = GetDocument();
	pDoc->SetTitle( sFTitle );

	int bicount;

	BITMAPFILEHEADER bmpFHeader;
	BITMAPINFOHEADER bmiHeader;

	fread( &bmpFHeader, sizeof(BITMAPFILEHEADER), 1, fpImg );
	fread( &bmiHeader, sizeof(BITMAPINFOHEADER), 1, fpImg );

	width = bmiHeader.biWidth;
	height = bmiHeader.biHeight;
	bicount = bmiHeader.biBitCount;


	BYTE* rowBuff;
	int rowLen;

	rowLen = ((width*bicount)/8+3)/4 *4;
	rowBuff = new BYTE [rowLen];

	if(bicount == 8)
	{
		RGBQUAD bmiColors[256];
		fread( bmiColors, sizeof(RGBQUAD), 256, fpImg );
		image = new BYTE [width*height];
		rgbimg = new BYTE [3*width*height];
		for( int i=height-1; i>=0; i-- )
		{
			fread( rowBuff, 1, rowLen, fpImg );
			memcpy( image+i*width, rowBuff, width );
		}
		for( int i=0; i<height; i++ )
			for(int j=0; j<3*width; j=j+3 )
			{
				rgbimg[i*3*width+j] = bmiColors[ image[i*width+j/3] ].rgbBlue;
				rgbimg[i*3*width+j+1] = bmiColors[ image[i*width+j/3] ].rgbGreen;
				rgbimg[i*3*width+j+2] = bmiColors[ image[i*width+j/3] ].rgbRed;
			}
	}
	if(bicount == 24)
	{
		image = new BYTE [width*height];
		rgbimg = new BYTE [rowLen*height];
		for( int i=height-1; i>=0; i-- )
		{
			fread( rowBuff, 1, rowLen, fpImg );
			memcpy( rgbimg+i*3*width, rowBuff, 3*width );
		}
	}
	delete rowBuff;

	fclose(fpImg);

////////////////////////////////////////////////////////////

    OnInitialUpdate();
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
			if(huiimg!=NULL)
			{
				huiimg=NULL;
			}
		}
	}
	if( nChar == VK_PRIOR )
	{
		if( findex > 0 )
		{
			findex--;
			readImg( findex );
			if(huiimg!=NULL)
			{
				huiimg=NULL;
			}
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
	sprintf( fname, "D:\\out%s.raw", str );

	FILE* fpOut;
	fpOut = fopen( fname, "wb" );

	fwrite( &width, 4, 1, fpOut );
	fwrite( &height, 4, 1, fpOut );
	fwrite( outImg, 1, width*height, fpOut );

	fclose( fpOut );

}



void CImgProView::Color()
{
	// TODO: 在此添加命令处理程序代码
    BYTE r,g,b;
	int i,j;
	double hg,hr;
	double max1=0.0;
	BYTE gray;
	gray = 0;
	flag = 1;
	OnInitialUpdate();

	huiimg = new BYTE[width * height];
	huiimg1 = new BYTE[width * height];
	huiimg2 = new BYTE[width * height];
	huiimg3 = new BYTE[width * height];
	BYTE max, min, delta;
	double h, s, v;
	int helllworld;

//******************************************
//2019.12.19潘先铧：将RGB转化到HSV
//******************************************

	for (int i = 0; i < height; i++)																
	{
		for (int j = 0; j < width; j++)
		{
			b = rgbimg[i * 3 * width + j * 3];
			g = rgbimg[i * 3 * width + j * 3 + 1];
			r = rgbimg[i * 3 * width + j * 3 + 2];

			max = (r > g) ? r : g;
			max = (max > b) ? max : b;

			min = (r < g) ? r : g;
			min = (min < b) ? min : b;

			delta = max - min;
			if (delta == 0)
				h = 0;
			else
			{
				if (max == r && g >= b)
					h = double(60 * (g - b)) / delta;
				if (max == r && g < b)
					h = double(60 * (g - b)) / delta + 360;
				if (max == g)
					h = double(60 * (b - r)) / delta + 120;
				if (max == b)
					h = double(60 * (r - g)) / delta + 240;
			}
//******************************************
//2019.12.19潘先铧：在HSV下利用图像颜色特征分割车牌
//得到的是车牌区域图像的二值化图像，车牌区域白色，其他黑色
//******************************************
			v = max;
			if (v == 0)
			{
				s = 0;
			}
			else
				s = double(delta) / max;
			if (h < 0)
				h += 360;
			if ((h <= 270.0 && 208.0 <= h) && s > 0.52)				//参数可根据实际情况调整，相较于下面的上面个这组参数效果较好
			{
				huiimg1[i * width + j] = 255;
			}
			else
			{
				huiimg1[i * width + j] = 0;
			}
			//if ((h <= 270.0 && 205.0 <= h) && s > 0.48)
			//{
				//huiimg[i * width + j] = 255;
			//}
			//else
			//{
				//huiimg[i * width + j] = 0;
			//}
			huiimg2[i * width + j] = huiimg1[i * width + j];
		}

	}

//******************************************
//2019.12.1沈灿红：对分割出来的图像进行腐蚀膨胀
//******************************************
	BYTE* tempout;
	BYTE* tempout2;
	tempout = new BYTE[width * height];
	tempout2 = new BYTE[width * height];
	
	erosion(huiimg1, width, height, tempout,1);         //first腐蚀then膨胀
	dilation(tempout, width, height, tempout2,2);
	erosion(tempout2, width, height, tempout, 1);
	dilation(tempout, width, height, huiimg1, 1);
	
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			huiimg3[i * width + j] = huiimg1[i * width + j];
		}
	}
//**************************************************************
//2019.12.20潘先铧，根据腐蚀膨胀图像判断车牌范围
//腐蚀膨胀中可能对大块污点处理不充分，
//根据污点长度和车牌长度比较将其直接从图像中去除。
//maxh,minh纵向范围，maxw,minw横向范围
//**************************************************************									
	maxw = height / 10; 
	maxh = width / 10; 
	minh = 9 * height / 10; 
	minw = 9 * width / 10;

	for (i = 0; i < height; i++) {
		int c = 0;
		for (j = 0; j < width; j++) {
			if (huiimg1[i * width + j] == 255)
				c = c + 1;                                           
		}
		if (c < width / 20) {
			for (j = 0; j < width; j++) {
				huiimg1[i * width + j] =0;
			}
		}
	}                                                          //去除大块污点


	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if (i > height * 3 / 10 && i < height * 19 / 22 && j>width * 3 / 10 && j < 19 * width / 22) {
				if (huiimg1[i * width + j] == 255 && j < minw) {
					minw = j;
				}
				if (huiimg1[i * width + j] == 255 && j > maxw) {
					maxw = j;
				}
			}
		}
	}                                                                      //find    minw,maxw

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if (i > height * 3 / 10 && i < height * 19 / 22 && j>width * 3 / 10 && j < 19 * width / 22) {
				if (huiimg1[i * width + j] == 255 && i < minh) {
					minh = i;
				}
				if (huiimg1[i * width + j] == 255 && i > maxh) {
					maxh = i;
				}
			}
		}
	}                                                                      //find    minh,maxh

	for (i = 0; i < height; i++) {											//根据找到的车牌范围处理图像
		for (j = 0; j < width; j++) {
			if (i >= minh && i <= maxh && j <= maxw && j >= minw) {
				huiimg1[i * width + j] = 255;
			}
			else {
				huiimg1[i * width + j] = 0;
			}
			huiimg[i * width + j] = huiimg1[i * width + j];
		}
	}
//2019.12.21刘皓郡，定义新的保存分割出来的车牌的数组
	newwidth = maxw - minw;
	newheight = maxh - minh;
	newrgbimg = new BYTE[3 * newwidth * newheight];
}

void CImgProView::dilation(BYTE* image, int w, int h, BYTE* outImg, int p)
{
	int rept;
	memcpy(outImg, image, sizeof(BYTE) * width * height);
	int i, j, m, n;
	BYTE flag;
	for (rept = 0; rept < p; rept++) {
		for (i = 1; i < h - 1; i++) {
			for (j = 1; j < w - 1; j++) {
				if (image[i * w + j] == 0) {
					flag = 0;
					for (m = -1; m < 2; m++) {
						for (n = -1; n < 2; n++) {
							if (image[(i + m) * w + j + n] == 255) {
								flag++;
								break;
							}
						}
					}
					if (flag > 0)
						outImg[i * w + j] = 255;
				}
			}
		}
		memcpy(image, outImg, sizeof(BYTE) * width * height);
	}
}

void CImgProView::erosion(BYTE* image, int w, int h, BYTE* outImg,int p)
{
	int rept;
	memcpy(outImg, image, sizeof(BYTE) * width * height);
	int i, j, m, n;
	BYTE flag;
	for (rept = 0; rept < p; rept++) {
		for (i = 1; i < h - 1; i++) {
			for (j = 1; j < w - 1; j++) {
				if (image[i * w + j] == 255) {
					flag = 0;
					for (m = -1; m < 2; m++) {
						for (n = -1; n < 2; n++) {
							if (image[(i + m) * w + j + n] == 0) {
								flag++;
								break;
							}
						}
					}
					if (flag > 0)
						outImg[i * w + j] = 0;
				}
			}
		}
		memcpy(image, outImg, sizeof(BYTE) * width * height);
	}
}

//*************************************************************
//2019.12.21高成鑫，车牌区域图像的提取
//*************************************************************

void CImgProView::Imgsegmentation()
{
	// TODO: 在此添加命令处理程序代码
	flag = 0;
	segflag = 1;
	int i, j;
	BYTE r, g, b;
	BYTE gray;
	newhuiimg = new BYTE[newwidth * newheight];
	OnInitialUpdate();
	if (huiimg)
	{
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				if (huiimg[i * width + j] != 0) 
				{
					b= rgbimg[i * 3 * width + j * 3];
					g= rgbimg[i * 3 * width + j * 3 + 1];
					r= rgbimg[i * 3 * width + j * 3 + 2];
					
					newrgbimg[(i - minh) * newwidth * 3 + (j - minw) * 3] = b;
					newrgbimg[(i - minh) * newwidth * 3 + (j - minw) * 3 + 1] = g;
					newrgbimg[(i - minh) * newwidth * 3 + (j - minw) * 3 + 2] = r;

					//newhuiimg[(i - minh) * newwidth + (j - minw)] = 0.110 * b + 0.588 * g + 0.302 * r;
				}
			}
		}
/*		for (i = 0; i < newheight; i++)
		{
			for (j = 0; j < newwidth; j++)
			{
				b = newrgbimg[i * newwidth * 3 + j * 3];
				g = newrgbimg[i * newwidth * 3 + j * 3 + 1];
				r = newrgbimg[i * newwidth * 3 + j * 3 + 2];
				newhuiimg[i*newwidth+j]=BYTE(0.110 * b + 0.588 * g + 0.302 * r);
			}
		}
*/
	}
}
//*************************************************************
//2019.12.22，高成鑫，车牌图像的扫描分割
//按比例分割图片
//容易受车牌倾斜、图片角度变化影响
//应该对图像进行处理，如霍夫变换纠正倾斜图像
//同理，分割时更好的处理方法应该是
//基于灰度图像对图像进行连通域分析
//*************************************************************

void CImgProView::imgdivided()
{
	// TODO: 在此添加命令处理程序代码
	divflag = 1;
	flag = 2;
	segflag = 1;
	OnInitialUpdate();
}
