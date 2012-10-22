// DesktopDlg.cpp : implementation file
//

#include "stdafx.h"
#include "global.h"
#include "DesktopIconAssistant.h"
#include "DesktopIconAssistantDlg.h"
#include "DesktopDlg.h"

// CDesktopDlg dialog

IMPLEMENT_DYNAMIC(CDesktopDlg, CDialog)

CDesktopDlg::CDesktopDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesktopDlg::IDD, pParent)
	, m_iToolType(NONE)
	, m_bSelectArea(FALSE)
	, m_iBezierPointNum(0)
{
	memset(&m_ptBegin, 0 , sizeof(POINT));
	memset(&m_ptEnd, 0 , sizeof(POINT));
	m_bmpBg.LoadBitmap(IDB_DESKTOP_BG);
}

CDesktopDlg::~CDesktopDlg()
{
	m_bmpBg.DeleteObject();
}

void CDesktopDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CDesktopDlg, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDOK, &CDesktopDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDesktopDlg message handlers

BOOL CDesktopDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	setFace();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDesktopDlg::setToolType(int type)
{
	if (type > NONE && type <= BEZIER)
	{
		m_iToolType = type;
	}
}

void CDesktopDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_bSelectArea)
	{
		beginSelect(point);
	}
	else
	{
		endSelect(point);
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CDesktopDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bSelectArea)
	{
		POINT pt;
		pt = point;
		if (MK_SHIFT & nFlags)
		{
			adjustPoint(&pt);
		}
		endSelect(pt);
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CDesktopDlg::drawOutline(CDC &dc)
{
	// Set the pen color
	CPen pen(PS_SOLID, 4, RGB(255, 0, 0));
	dc.SelectObject(&pen);
	dc.SelectObject(GetStockObject(NULL_BRUSH));

	switch (m_iToolType)
	{
	case LINE:
		dc.MoveTo(m_ptBegin);
		dc.LineTo(m_ptEnd);
		break;
	case HOLLOWRECT:
		// jump to case FILLRECT
	case FILLRECT:
		dc.Rectangle(m_ptBegin.x, m_ptBegin.y, m_ptEnd.x, m_ptEnd.y);
		break;
	case OVAL:
		dc.Ellipse(m_ptBegin.x, m_ptBegin.y, m_ptEnd.x, m_ptEnd.y);
		break;
	case HEART:
		RECT rct;
		rct.left = min(m_ptBegin.x, m_ptEnd.x);
		rct.top = min(m_ptBegin.y, m_ptEnd.y);
		rct.right = max(m_ptBegin.x, m_ptEnd.x);
		rct.bottom = max(m_ptBegin.y, m_ptEnd.y);
		m_heart.setRect(&rct);
		m_heart.draw(dc);
		break;
	case BEZIER:
		if (m_iBezierPointNum <= 2)
		{
			dc.MoveTo(m_aBezierPoints[0]);
			dc.LineTo(m_aBezierPoints[3]);
		}
		else if (m_iBezierPointNum <= 3)
		{
			m_aBezierPoints[2] = m_aBezierPoints[1];
			dc.PolyBezier(m_aBezierPoints, 4);
		}
		else
		{
			dc.PolyBezier(m_aBezierPoints, 4);
		}
		break;
	default:
		break;
	}
}

void CDesktopDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CDC bgDC;
	CRect rect;
	BITMAP bit;

	GetClientRect(&rect);

	// load the background bitmap
	m_bmpBg.GetBitmap(&bit);
	bgDC.CreateCompatibleDC(&dc);
	bgDC.SelectObject(&m_bmpBg);

	// copy the background bitmap to the buffer dc
	dc.StretchBlt(0, 0, rect.right - rect.left, rect.bottom - rect.left, &bgDC, 0, 0, bit.bmWidth, bit.bmHeight, SRCCOPY);

	CString msg;
	CString sFormat;
	CFont mFont;
	mFont.CreateFont(30, 15, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("ËÎÌו"));
	dc.SetBkMode(TRANSPARENT);
	sFormat.LoadString(IDS_ICONS_SELECTED);
	msg.Format(sFormat, ((CDesktopIconAssistantDlg *)AfxGetApp()->m_pMainWnd)->getSelectedIconCount());
	dc.SetTextColor(RGB(255, 0, 0));
	dc.SelectObject(&mFont);
	dc.TextOut(rect.right / 2 - 120, rect.bottom / 2, msg, msg.GetLength());

	drawOutline(dc);
}

void CDesktopDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (MK_LBUTTON & nFlags)
	{
		if (m_bSelectArea)
		{
			POINT pt;
			pt = point;
			if (MK_SHIFT & nFlags)
			{
				adjustPoint(&pt);
			}
			if (m_iToolType == BEZIER)
			{
				switch(m_iBezierPointNum)
				{
				case 1:
					m_aBezierPoints[3] = pt;
					break;
				case 3: 
					m_aBezierPoints[1] = pt;
					break;
				case 4:
					m_aBezierPoints[2] = pt;
					break;
				default:
					break;
				}			
			}
			else
			{
				m_ptEnd = pt;
			}
			Invalidate();
		}
	} 
	else
	{
		if (m_bSelectArea)
		{
			endSelect(m_ptEnd);
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}


void CDesktopDlg::beginSelect(POINT pt)
{	
	m_bSelectArea = TRUE;

	if (m_iToolType == BEZIER)
	{
		if (m_iBezierPointNum >= 4)
			m_iBezierPointNum = 1;
		else
			m_iBezierPointNum++;
		switch(m_iBezierPointNum)
		{
		case 1:
			m_aBezierPoints[0] = pt;
			break;
		case 3:
			m_aBezierPoints[1] = pt;
			break;
		case 4:
			m_aBezierPoints[2] = pt;
			break;
		default:
			break;
		}
	} 
	else
	{
		m_ptBegin = pt;
	}

	// hide all the buttons
	m_btnOK.ShowWindow(SW_HIDE);
	m_btnCancel.ShowWindow(SW_HIDE);

}

void CDesktopDlg::endSelect(POINT pt)
{
	m_bSelectArea = FALSE;
	
	if (m_iToolType == BEZIER)
	{
		switch(m_iBezierPointNum)
		{
		case 1:
			m_iBezierPointNum++;
			m_aBezierPoints[3] = pt;
			break;
		case 3: 
			m_aBezierPoints[1] = pt;
			break;
		case 4:
			m_aBezierPoints[2] = pt;
			break;
		default:
			break;
		}
	} 
	else
	{
		m_ptEnd = pt;
	}

	// Show all the buttons
	m_btnOK.ShowWindow(SW_SHOWNORMAL);
	m_btnCancel.ShowWindow(SW_SHOWNORMAL);

	// Draw the outline
	Invalidate();

	if (m_iToolType == BEZIER && m_iBezierPointNum == 4)
		((CDesktopIconAssistantDlg *)AfxGetApp()->m_pMainWnd)->bezierSort(m_aBezierPoints);
	else
		((CDesktopIconAssistantDlg *)AfxGetApp()->m_pMainWnd)->toolSort(m_ptBegin, m_ptEnd, m_iToolType);
}

void CDesktopDlg::adjustPoint(LPPOINT lpPoint)
{
	double dx = fabs((double)m_ptBegin.x - lpPoint->x) + EPS;
	double dy = fabs((double)m_ptBegin.y - lpPoint->y) + EPS;
	switch (m_iToolType)
	{
	case LINE:
		if (dx / dy > 2)
		{
			lpPoint->y = m_ptBegin.y;
		} 
		else if (dy / dx > 2)
		{
			lpPoint->x = m_ptBegin.x;
		} 
		else if (dx / dy >= 0.5 
			&& dx / dy <= 2)
		{
			if (dx < dy)
			{
				lpPoint->y = (long)(m_ptBegin.y + (lpPoint->y - m_ptBegin.y) * dx / dy);
			}
			else
			{
				lpPoint->x = (long)(m_ptBegin.x + (lpPoint->x - m_ptBegin.x) * dy / dx);;
			}
		}
		break;
	case HOLLOWRECT:
		// jump to case OVAL
	case FILLRECT:
		// jump to case OVAL
	case OVAL:
		if (dx < dy)
		{
			lpPoint->y = (long)(m_ptBegin.y + (lpPoint->y - m_ptBegin.y) * dx / dy);
		}
		else
		{
			lpPoint->x = (long)(m_ptBegin.x + (lpPoint->x - m_ptBegin.x) * dy / dx);
		}
		break;
	case HEART:
		if (dx * 1.082 < dy)
		{
			lpPoint->y = (long)(m_ptBegin.y + (lpPoint->y - m_ptBegin.y) * dx * 1.082 / dy);
		}
		else
		{
			lpPoint->x = (long)(m_ptBegin.x + (lpPoint->x - m_ptBegin.x) * dy / 1.082 / dx);
		}
	default:
		break;
	}
}

void CDesktopDlg::setFace(void)
{
	// Load the OK button
	m_btnOK.SetBitmaps(IDB_OK_NORMAL, 0, IDB_OK_OVER, 0, RGB(0, 0, 255));
	m_btnOK.SizeToContent();

	// Load the OK button
	m_btnCancel.SetBitmaps(IDB_CANCEL_NORMAL, 0, IDB_CANCEL_OVER, 0, RGB(0, 0, 255));
	m_btnCancel.SizeToContent();

	// Get the desktop area
	RECT rctDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rctDesktop, 0);

	// Change the dialog size to cover the whole desktop
	MoveWindow(&rctDesktop);

	RECT rctClient;
	GetClientRect(&rctClient);	
	int x = rctClient.right / 2 - 100; 
	int y = rctClient.bottom / 2 - 100;

	// Move the buttons to the center
	m_btnOK.SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_btnCancel.SetWindowPos(NULL, x + 60 + 30, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Make the dialog transparent
	SetLayeredWindowAttributes(0, 200, LWA_ALPHA);
}

BOOL CDesktopDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

void CDesktopDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}