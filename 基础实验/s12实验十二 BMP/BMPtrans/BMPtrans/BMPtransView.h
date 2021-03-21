// BMPtransView.h : interface of the CBMPtransView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BMPTRANSVIEW_H__C69C2C0C_55CC_11D4_B8CE_D7550332222F__INCLUDED_)
#define AFX_BMPTRANSVIEW_H__C69C2C0C_55CC_11D4_B8CE_D7550332222F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBMPtransView : public CView
{
	BYTE* image;
	int width, height;
	char imgFName[80];

	BYTE *rgbImg;
	BYTE bmpflag;
protected: // create from serialization only
	CBMPtransView();
	DECLARE_DYNCREATE(CBMPtransView)

// Attributes
public:
	CBMPtransDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBMPtransView)
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
	virtual ~CBMPtransView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CBMPtransView)
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in BMPtransView.cpp
inline CBMPtransDoc* CBMPtransView::GetDocument()
   { return (CBMPtransDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPTRANSVIEW_H__C69C2C0C_55CC_11D4_B8CE_D7550332222F__INCLUDED_)
