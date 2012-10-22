#pragma once
#include "heart.h"
#include "BitButtonNL.h"
#include "afxwin.h"


// CDesktopDlg dialog

class CDesktopDlg : public CDialog
{
	DECLARE_DYNAMIC(CDesktopDlg)

public:
	CDesktopDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDesktopDlg();

// Dialog Data
	enum { IDD = IDD_DESKTOP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CBitButtonNL m_btnOK;
	CBitButtonNL m_btnCancel;
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedOk();
private:
	int m_iToolType;
	int m_iBezierPointNum;
	POINT m_ptBegin;
	POINT m_ptEnd;
	POINT m_aBezierPoints[4];
	BOOL m_bSelectArea;
	CHeart m_heart;
	CBitmap m_bmpBg;
public:
	void setToolType(int type);
private:
	void drawOutline(CDC &dc);
	void beginSelect(POINT pt);
	void endSelect(POINT pt);
	void adjustPoint(LPPOINT lpPoint);
	void setFace(void);
};
