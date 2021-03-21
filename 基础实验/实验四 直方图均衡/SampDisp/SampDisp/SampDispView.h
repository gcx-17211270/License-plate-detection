// sampDispView.h : interface of the CSampDispView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPDISPVIEW_H__C1754DF1_085E_4718_9654_CA4BBA11DD5E__INCLUDED_)
#define AFX_SAMPDISPVIEW_H__C1754DF1_085E_4718_9654_CA4BBA11DD5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSampDispView : public CView
{
protected: // create from serialization only
	CSampDispView();
	DECLARE_DYNCREATE(CSampDispView)

	// Attributes
public:
	CSampDispDoc* GetDocument();

	// Operations
public:
	BYTE* data;
	int sampNum;
	int dim;

	BYTE eqlFlag;

	int rowNum, colNum;
	int pageNum;
	int rowStep, colStep;

	void drawImg(CDC*, BYTE*, int, int, int);

	void hisEqualiz(BYTE*, int, int, BYTE*);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampDispView)
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
	virtual ~CSampDispView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CSampDispView)
	afx_msg void OnFileOpen();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in sampDispView.cpp
inline CSampDispDoc* CSampDispView::GetDocument()
{
	return (CSampDispDoc*)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAMPDISPVIEW_H__C1754DF1_085E_4718_9654_CA4BBA11DD5E__INCLUDED_)