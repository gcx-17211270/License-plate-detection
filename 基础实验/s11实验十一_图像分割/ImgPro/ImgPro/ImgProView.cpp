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
	BackImg = 0;
	SubImg = 0;
	findex = 0;
	BinImg = 0;
	pengImg = 0;
	SegmImg = 0;
	tranImg = 0;

}

CImgProView::~CImgProView()
{
	if (image)
		delete image;
	if (BackImg)
		delete BackImg;
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

	if (image == 0)
		return;

	int i, j;
	BYTE gray;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			gray = image[i * width + j];
			pDC->SetPixel(j, i, RGB(gray, gray, gray));
			gray = BackImg[i * width + j];
			pDC->SetPixel(width + 10 + j, i, RGB(gray, gray, gray));
			//////   show outImg here /////
			gray = SubImg[i * width + j];
			pDC->SetPixel(2 * width + 20 + j, i, RGB(gray, gray, gray));
			gray = BinImg[i * width + j];
			pDC->SetPixel(j, i + height + 10, RGB(gray, gray, gray));

			gray = SegmImg[i * width + j];
			pDC->SetPixel(width + 10 + j, i + height + 10, RGB(gray, gray, gray));

			gray = pengImg[i * width + j];
			pDC->SetPixel(2 * width + 20 + j, i + height + 10, RGB(gray, gray, gray));
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
	CFileDialog MyFDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, NULL, NULL);

	MyFDlg.m_ofn.lpstrFilter = "(*.lst;*.raw)\0*.lst;*.raw\0";
	MyFDlg.m_ofn.lpstrInitialDir = "D:\\imgPro";

	if (MyFDlg.DoModal() == IDOK)
	{
		CString SFName;

		SFName = MyFDlg.GetPathName(); //full name with path

		char* fnstr;
		fnstr = SFName.GetBuffer(4); //read the name from string 

		FILE* fpLst;
		int n;

		int len;
		len = strlen(fnstr);
		if (!strcmp(fnstr + len - 3, "raw")) //single raw image
		{
			fnum = 0;
			char* ptr;
			ptr = fnstr + len - 3;
			while (*ptr != '\\')
				ptr--;
			*ptr = 0;
			strcpy(directory, fnstr);//Directory

			fnames = new char[500];
			strcpy(fnames, ptr + 1); //image name
		}
		else //list file
		{
			fpLst = fopen(fnstr, "rb");
			fscanf(fpLst, "%3d", &fnum);
			fscanf(fpLst, "%s", directory);//directory

			fnames = new char[fnum * 100];
			for (n = 0; n < fnum; n++)
				fscanf(fpLst, "%s", fnames + n * 100);// image names

			fclose(fpLst);
		}

		findex = 0;
		readImg(findex);
	}
}

void CImgProView::readImg(int findex)
{

	char fullName[500];
	sprintf(fullName, "%s\\%s", directory, fnames + findex * 100);

	FILE* fpImg;
	fpImg = fopen(fullName, "rb");
	if (fpImg == 0)
	{
		AfxMessageBox("Cannot open the image file ", MB_OK, 0);
		return;
	}

	CString sFTitle;
	sFTitle.Format("%s", fnames + findex * 100);

	CImgProDoc* pDoc;
	pDoc = GetDocument();
	pDoc->SetTitle(sFTitle);

	fread(&width, sizeof(int), 1, fpImg);
	fread(&height, sizeof(int), 1, fpImg);

	if (image)
		delete image;

	image = new BYTE[width * height];
	fread(image, sizeof(BYTE), width * height, fpImg);
	fclose(fpImg);

	findex++;
	sprintf(fullName, "%s\\%s", directory, fnames + findex * 100);
	fpImg = fopen(fullName, "rb");
	if (fpImg == 0)
	{
		AfxMessageBox("Cannot open the image file", MB_OK, 0);
		return;
	}

	fread(&width, sizeof(int), 1, fpImg);
	fread(&height, sizeof(int), 1, fpImg);

	if (BackImg)
		delete BackImg;

	BackImg = new BYTE[width * height];
	fread(BackImg, sizeof(BYTE), width * height, fpImg);
	fclose(fpImg);


	////////  add processing function here修改  ///////////////////
	SubImg = new BYTE[width * height];
	subtract(image, width, height, BackImg, SubImg);
	BinImg = new BYTE[width * height];
	binarize(SubImg, width, height, BinImg);
	tranImg = new BYTE[width * height];
	tranImg2 = new BYTE[width * height];
	memcpy(tranImg, BinImg, sizeof(BYTE) * width * height);
	SegmImg = new BYTE[width * height];
	erosion(tranImg, width, height, SegmImg);
	memcpy(tranImg2, SegmImg, sizeof(BYTE) * width * height);
	pengImg = new BYTE[width * height];
	dilation(tranImg2, width, height, pengImg);



	////////////////////////////////////////////////////////////

	OnInitialUpdate();
	CRect ClientRect;
	GetClientRect(&ClientRect);
	InvalidateRect(&ClientRect);

}



void CImgProView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == VK_NEXT)
	{
		if (findex < fnum)
		{
			findex++;
			readImg(findex);
		}
	}
	if (nChar == VK_PRIOR)
	{
		if (findex > 0)
		{
			findex--;
			readImg(findex);
		}
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CImgProView::OnFileSave()
{
	// TODO: Add your command handler code here
	char str[40], * ptr;
	sprintf(str, fnames + findex * 40);
	ptr = str;
	while (*ptr != '.')
		ptr++;
	*ptr = 0;

	char fname[80];
	sprintf(fname, "D:\\Teaching\\ImagePro\\bb\\out%s.raw", str);

	FILE* fpOut;
	fpOut = fopen(fname, "wb");

	fwrite(&width, 4, 1, fpOut);
	fwrite(&height, 4, 1, fpOut);
	fwrite(SegmImg, 1, width * height, fpOut);

	fclose(fpOut);

}
void CImgProView::subtract(BYTE* image, int width, int height, BYTE* BackImg, BYTE* SubImg)
{
	int i, j;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (image[i * width + j] >= BackImg[i * width + j]) SubImg[i * width + j] = image[i * width + j] - BackImg[i * width + j];
			else  SubImg[i * width + j] = -image[i * width + j] + BackImg[i * width + j];
		}

	}
}



void CImgProView::binarize(BYTE* SubImg, int width, int height, BYTE* BinImg)  //二值化
{
	int i, j;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (SubImg[i * width + j] > 30)
				BinImg[i * width + j] = 255;
			else
				BinImg[i * width + j] = 0;
		}
	}
	for (i = 0; i < height / 2; i++)
	{
		for (j = 0; j < width / 2 - 35; j++)
		{
			BinImg[i * width + j] = 0;
		}
	}
}

void CImgProView::erosion(BYTE* tranImg, int width, int height, BYTE* SegmImg)  //二值化
{
	int rept;
	memcpy(SegmImg, tranImg, sizeof(BYTE) * width * height);

	int i, j;

	int m, n;
	BYTE flag;
	for (rept = 0; rept < 1; rept++)
	{
		for (i = 0; i <= height; i++)
		{
			for (j = 0; j <= width; j++)
			{
				if (tranImg[i * width + j] == 255)
				{
					flag = 0;
					for (m = -1; m < 2; m++)
					{
						for (n = -1; n < 2; n++)
						{
							if (tranImg[(i + m) * width + j + n] == 0)
							{
								flag++;
								break;
							}
						}
					}
					if (flag > 0)
						SegmImg[i * width + j] = 0;

				}
			}
		}
		memcpy(tranImg, SegmImg, sizeof(BYTE) * width * height);
	}
}

void CImgProView::dilation(BYTE* tranImg2, int width, int height, BYTE* pengImg)//膨胀
{
	int rept;
	memcpy(pengImg, tranImg2, sizeof(BYTE) * width * height);

	int i, j;
	int m, n;
	BYTE flag;
	for (rept = 0; rept < 6; rept++)
	{
		for (i = 2; i < height - 2; i++)
		{
			for (j = 1; j < width - 1; j++)
			{
				if (tranImg2[i * width + j] == 0)
				{
					flag = 0;
					for (m = -1; m < 2; m++)
					{
						for (n = -1; n < 2; n++)
						{
							if (tranImg2[(i + m) * width + j + n] == 255)

								flag++;


						}
					}

					if (flag >= 1)
						pengImg[i * width + j] = 255;
				}
			}
		}
		memcpy(tranImg2, pengImg, sizeof(BYTE) * width * height);
	}
}