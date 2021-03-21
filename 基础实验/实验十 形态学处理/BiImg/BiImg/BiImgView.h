// BiImgView.h : interface of the CBiImgView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BIIMGVIEW_H__05F24F9F_A2A8_4B11_994B_24BB4AED87B9__INCLUDED_)
#define AFX_BIIMGVIEW_H__05F24F9F_A2A8_4B11_994B_24BB4AED87B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBiImgView : public CView
{
protected: // create from serialization only
	CBiImgView();
	DECLARE_DYNCREATE(CBiImgView)

// Attributes
public:
	CBiImgDoc* GetDocument();

	int width, height;
	BYTE *image;

	void readImg( int );
	int findex;
	int fnum;
	char directory[120];
	char *fnames;

    BYTE *outImg1;
    BYTE *outImg2;
				
	void erosion( BYTE*, int, int, BYTE*);
	void dilation( BYTE*, int, int, BYTE*);

	BYTE LButtonDown;
	BYTE RButtonDown;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBiImgView)
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
	virtual ~CBiImgView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CBiImgView)
	afx_msg void OnFileOpen();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in BiImgView.cpp
inline CBiImgDoc* CBiImgView::GetDocument()
   { return (CBiImgDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BIIMGVIEW_H__05F24F9F_A2A8_4B11_994B_24BB4AED87B9__INCLUDED_)
