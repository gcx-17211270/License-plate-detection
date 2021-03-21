// ImgProView.h : interface of the CImgProView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMGPROVIEW_H__CA52376A_801B_4E87_AA91_71B205388883__INCLUDED_)
#define AFX_IMGPROVIEW_H__CA52376A_801B_4E87_AA91_71B205388883__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "math.h"
typedef unsigned char BYTE;

class CImgProView : public CView
{
protected: // create from serialization only
	CImgProView();
	DECLARE_DYNCREATE(CImgProView)

// Attributes
public:
	CImgProDoc* GetDocument();

// Operations
public:

	BYTE *image;
	BYTE *outImg;
	BYTE *rgbimg;
//***********************************************
//用来保存分割出来的车牌图像
	BYTE* newrgbimg;
//用来保存新的分割出来的车牌图像的灰度图像
	BYTE* newhuiimg;
//***********************************************
	BYTE *huiimg;

//***********************************************
//以下的huiimgx都是在处理过程中图像的模样，用于显示
//huiimg用来保存最后的输出图像结果
	BYTE* huiimg1;
	BYTE* huiimg2;
	BYTE* huiimg3;
//***********************************************

	int width, height;

//***********************************************
//用来保存新的分割出来的车牌的宽和高
	int newwidth, newheight;
	int maxw, minw, maxh, minh;
//***********************************************
	void readImg( int );
	int findex;
	int fnum;
	char directory[120];
	char *fnames;
	int bmpflag,rawflag;
	int flag;
//******************************
//2019.12.21高成鑫，图像分割标志位
//扫描分割标志位
	int segflag;
	int divflag;
//******************************


    // add image processing function 


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImgProView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CImgProView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CImgProView)
	afx_msg void OnFileOpen();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnFileSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void Color();
	void dilation(BYTE* image, int w, int h, BYTE* outImg, int p);
	void erosion(BYTE* image, int w, int h, BYTE* outImg, int p);
	afx_msg void Imgsegmentation();
	afx_msg void imgdivided();
};

#ifndef _DEBUG  // debug version in ImgProView.cpp
inline CImgProDoc* CImgProView::GetDocument()
   { return (CImgProDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMGPROVIEW_H__CA52376A_801B_4E87_AA91_71B205388883__INCLUDED_)
