/////////////////////////////////////////////////////////////////////////////
/*
 *	BitButtonNL.cpp : implementation file
 *
 *					Author Ning Li
 *		 			     2004-5-28
 */
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BitButtonNL.h"

#ifdef	BTNNL_USE_SOUND
#pragma comment(lib, "winmm.lib")
#include <Mmsystem.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBitButtonNL

CBitButtonNL::CBitButtonNL()
{
	m_bIsPressed		= FALSE;
	m_bIsFocused		= FALSE;
	m_bIsDisabled		= FALSE;
	m_bMouseOnButton	= FALSE;
	m_bDrawFlatFocus	= FALSE;
	m_bDrawText			= FALSE;
	m_bDrawDisableText	= TRUE;
	m_ptPressedOffset.x = 1;
	m_ptPressedOffset.y = 1;
	m_iDeflateRect_x	= 3;
	m_iDeflateRect_y	= 3;
	m_nTypeStyle		= SS_TYPEMASK;
	m_bIsDefault		= FALSE;
	m_hCursor			= NULL;
	m_byTextAlign		= NL_ALIGN_CENTER;
	
	m_bFont3d			= FALSE;
	m_cr3DBKColor		= RGB(0, 0, 0);
	m_iText3d_x			= 3;
	m_iText3d_y			= 2;
	FreeResources(FALSE);
	m_crTrans			= RGB(0, 0, 0);
	m_bChecked			= FALSE;
#ifdef	BTNNL_USE_SOUND
	// No defined sounds
	::ZeroMemory(&m_csSounds, sizeof(m_csSounds));
#endif

}

CBitButtonNL::~CBitButtonNL()
{
	FreeResources();
	m_font.DeleteObject();
	if (m_hCursor) ::DestroyCursor(m_hCursor);

}


BEGIN_MESSAGE_MAP(CBitButtonNL, CButton)
	//{{AFX_MSG_MAP(CBitButtonNL)
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
	ON_WM_KILLFOCUS()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitButtonNL message handlers

void CBitButtonNL::FreeResources(BOOL bCheckForNULL)
{
	if (bCheckForNULL)
	{
		// Destroy bitmaps
		if (m_csBitmaps[0].hBitmap)	::DeleteObject(m_csBitmaps[0].hBitmap);
		if (m_csBitmaps[1].hBitmap)	::DeleteObject(m_csBitmaps[1].hBitmap);
		if (m_csBitmaps[2].hBitmap)	::DeleteObject(m_csBitmaps[2].hBitmap);
		if (m_csBitmaps[3].hBitmap)	::DeleteObject(m_csBitmaps[3].hBitmap);
	} // if

	::ZeroMemory(&m_csBitmaps, sizeof(m_csBitmaps));
} // End of FreeResources

void CBitButtonNL::PreSubclassWindow() 
{
	UINT nBS;

	nBS = GetButtonStyle();

	// Set initial control type
	m_nTypeStyle = nBS & SS_TYPEMASK;

	// Set initial default state flag
	if (m_nTypeStyle == BS_DEFPUSHBUTTON)
	{
		// Set default state for a default button
		m_bIsDefault = TRUE;

		// Adjust style for default button
		m_nTypeStyle = BS_PUSHBUTTON;
	} // If

	// You should not set the Owner Draw before this call
	// (don't use the resource editor "Owner Draw" or
	// ModifyStyle(0, BS_OWNERDRAW) before calling PreSubclassWindow() )
	if(m_nTypeStyle != BS_OWNERDRAW)
		// Switch to owner-draw
		ModifyStyle(SS_TYPEMASK, BS_OWNERDRAW, SWP_FRAMECHANGED);


	CFont* cf = GetFont();
	if(cf !=NULL)
	{
		cf->GetObject(sizeof(m_lf),&m_lf);
	}
	else
	{
		GetObject(GetStockObject(SYSTEM_FONT),sizeof(m_lf),&m_lf);
	}

	ReconstructFont();
	
	CButton::PreSubclassWindow();
}

void CBitButtonNL::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC*	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	m_bIsPressed = (lpDrawItemStruct->itemState & ODS_SELECTED);
	if (m_bChecked)
		m_bIsPressed = TRUE;

	m_bIsFocused  = (lpDrawItemStruct->itemState & ODS_FOCUS);
	m_bIsDisabled = (lpDrawItemStruct->itemState & ODS_DISABLED);

	CRect itemRect = lpDrawItemStruct->rcItem;

	pDC->SetBkMode(TRANSPARENT);

	// Prepare draw... paint button background

	// Read the button's title 
	CString sTitle;
	GetWindowText(sTitle);

	CRect captionRect = lpDrawItemStruct->rcItem;

	if (m_csBitmaps[0].hBitmap)
	{
		pDC->SetBkColor(m_crTrans);
		DrawTheBitmap(pDC, !sTitle.IsEmpty(), &lpDrawItemStruct->rcItem, &captionRect, m_bIsPressed, m_bIsDisabled);
	} // if

	
	// Write the button title (if any)
	if (!sTitle.IsEmpty() && m_bDrawText) 
	{
		DrawTheText(pDC, (LPCTSTR)sTitle, &lpDrawItemStruct->rcItem, &captionRect, m_bIsPressed, m_bIsDisabled);
	} // if

	if (m_bDrawFlatFocus)
	{
		// Draw the focus rect
		if (m_bIsFocused)
		{
			CRect focusRect = itemRect;
				focusRect.DeflateRect(m_iDeflateRect_x, m_iDeflateRect_y) ;
			if (m_bIsPressed)
			{
				focusRect.left	+= m_ptPressedOffset.x;
				focusRect.right += m_ptPressedOffset.x;
				focusRect.top	+= m_ptPressedOffset.y;
				focusRect.bottom+= m_ptPressedOffset.x;
			}
			pDC->SetTextColor(RGB(255,255,255));
			pDC->SetBkColor(RGB(0,0,0));
			pDC->DrawFocusRect(&focusRect);
		} // if
	} // if}
}

void CBitButtonNL::DrawTheBitmap(CDC* pDC, BOOL bHasTitle, RECT* rpItem, CRect* rpCaption, BOOL bIsPressed, BOOL bIsDisabled)
{
	HDC			hdcBmpMem	= NULL;
	HBITMAP		hbmOldBmp	= NULL;

	BYTE		byIndex		= 0;

	// Select the bitmap to use
	if (bIsDisabled)
		byIndex = m_csBitmaps[3].hBitmap == NULL ? 0 : 3;
	else if (bIsPressed)
		byIndex = m_csBitmaps[1].hBitmap == NULL ? 0 : 1;
	else if ((!bIsPressed) && m_bMouseOnButton)
		byIndex = m_csBitmaps[2].hBitmap == NULL ? 0 : 2;
	else
		byIndex = 0;

	CRect rImage;
	rImage.CopyRect(rpItem);

	if (bIsPressed && (byIndex == 0) )
		rImage.OffsetRect(m_ptPressedOffset.x, m_ptPressedOffset.y);

	hdcBmpMem = ::CreateCompatibleDC(pDC->m_hDC);

	hbmOldBmp = (HBITMAP)::SelectObject(hdcBmpMem, m_csBitmaps[byIndex].hBitmap);

	if (bIsDisabled && (byIndex == 0) )
	{
		HDC		hDC = NULL;
		HBITMAP	hBitmap = NULL;

		hDC = ::CreateCompatibleDC(pDC->m_hDC);
		hBitmap = ::CreateCompatibleBitmap(pDC->m_hDC, m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight);
		HBITMAP	hOldBmp2 = (HBITMAP)::SelectObject(hDC, hBitmap);

		RECT	rRect;
		rRect.left = 0;
		rRect.top = 0;
		rRect.right = rImage.right + 1;
		rRect.bottom = rImage.bottom + 1;
		::FillRect(hDC, &rRect, (HBRUSH)RGB(255, 255, 255));

		::TransparentBlt(hDC, 0, 0, m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight
				, hdcBmpMem, 0, 0, m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight, m_crTrans);

		::SetBkColor(hDC, m_crTrans);
		
		::SelectObject(hDC, hOldBmp2);
		::DeleteDC(hDC);

		pDC->DrawState(	CPoint(rImage.left/*+1*/, rImage.top), 
						CSize(m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight), 
						hBitmap, DST_BITMAP | DSS_DISABLED);

		::DeleteObject(hBitmap);
	} // if
	else
	{
		::TransparentBlt(pDC->m_hDC,rImage.left, rImage.top, m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight, hdcBmpMem, 0, 0, m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight, m_crTrans);
	} // else

	::SelectObject(hdcBmpMem, hbmOldBmp);
	::DeleteDC(hdcBmpMem);
} // End of DrawTheBitmap

void CBitButtonNL::DrawTheText(CDC* pDC, LPCTSTR lpszText, RECT* rpItem, CRect* rpCaption, BOOL bIsPressed, BOOL bIsDisabled)
{
	// Draw the button's title
	// If button is pressed then "press" title also
	if (m_bIsPressed)
		rpCaption->OffsetRect(m_ptPressedOffset.x, m_ptPressedOffset.y);

	// ONLY FOR DEBUG 
	//CBrush brBtnShadow(RGB(255, 0, 0));
	//pDC->FrameRect(rCaption, &brBtnShadow);

	// Center text
	CRect centerRect = rpCaption;

	// Save old mode
	UINT nMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF crText = pDC->SetTextColor(m_crForceColors[BTNNL_COLOR_FG_CLICK]);
	COLORREF crBack = pDC->SetBkColor(m_crForceColors[BTNNL_COLOR_FG_CLICK]);
	CFont *pOldFont = pDC->SelectObject(&m_font);
	
	pDC->DrawText(lpszText, -1, rpCaption, DT_WORDBREAK | DT_CENTER | DT_CALCRECT);

	switch(m_byTextAlign)
	{
	case NL_ALIGN_LEFT:
		rpCaption->OffsetRect((centerRect.Width() - rpCaption->Width()) / 5, (centerRect.Height() - rpCaption->Height()) / 2 - 1);
		break;
	case NL_ALIGN_RIGHT:
		rpCaption->OffsetRect((centerRect.Width() - rpCaption->Width()) * 4 / 5, (centerRect.Height() - rpCaption->Height()) / 2 - 1);
		break;
	case NL_ALIGN_CENTER:
		rpCaption->OffsetRect((centerRect.Width() - rpCaption->Width()) / 2, (centerRect.Height() - rpCaption->Height()) / 2 - 1);
		break;
	default:
		;
	}

	if (m_bIsDisabled && m_bDrawDisableText)
	{
		rpCaption->OffsetRect(1, 1);
		pDC->SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
		pDC->DrawText(lpszText, -1, rpCaption, DT_WORDBREAK | DT_CENTER);
		rpCaption->OffsetRect(-1, -1);
		pDC->SetTextColor(::GetSysColor(COLOR_3DSHADOW));
		pDC->DrawText(lpszText, -1, rpCaption, DT_WORDBREAK | DT_CENTER);
	} // if
	else
	{

		if (m_bIsPressed)
		{
			pDC->SetTextColor(m_crForceColors[BTNNL_COLOR_FG_CLICK]);
			pDC->SetBkColor(m_crForceColors[BTNNL_COLOR_FG_CLICK]);
		} // if
		else 
		{
			//3D
			if (m_bFont3d)
			{
				centerRect = rpCaption;
				pDC->SetTextColor(m_cr3DBKColor);

				centerRect.OffsetRect(m_iText3d_x, m_iText3d_y);

				pDC->DrawText(lpszText,-1, centerRect, DT_WORDBREAK | DT_CENTER);
			}
			if (m_bMouseOnButton) 
			{ 
				pDC->SetTextColor(m_crForceColors[BTNNL_COLOR_FG_HIGH]); 
				pDC->SetBkColor(m_crForceColors[BTNNL_COLOR_FG_HIGH]); 
			} // if
			else 
			{
				pDC->SetTextColor(m_crForceColors[BTNNL_COLOR_FG_NORMAL]); 
				pDC->SetBkColor(m_crForceColors[BTNNL_COLOR_FG_NORMAL]); 
			} // else
		} // else

		pDC->DrawText(lpszText,-1, rpCaption, DT_WORDBREAK | DT_CENTER);
	} // if
	
	// Restore DC's State
	pDC->SetBkMode(nMode);
	pDC->SelectObject(pOldFont);
	pDC->SetTextColor(crText);
	pDC->SetBkColor(crBack);
} // End of DrawTheText

void CBitButtonNL::SizeToContent(void)
{
	if (m_csBitmaps[0].hBitmap)
	{
		// Resize the button
	    SetWindowPos(NULL, -1, -1, m_csBitmaps[0].dwWidth, m_csBitmaps[0].dwHeight,
						SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);

		CClientDC dc(this);
		CDC dcMem;

		if (NULL == dc.m_hDC)
			return;

		dcMem.CreateCompatibleDC(&dc);

		dcMem.SelectObject(m_csBitmaps[0].hBitmap);

		CRgn wndRgn;	//创建总的窗体区域
		CRgn rgnTemp;
		wndRgn.CreateRectRgn(0,0,0,0);
		for(DWORD y=0; y<m_csBitmaps[0].dwHeight; y++)
		{
			for (DWORD x=0; x < m_csBitmaps[0].dwWidth; x++)
			{
				//跳过透明色找到下一个非透明色的点.
				while (x < m_csBitmaps[0].dwWidth && dcMem.GetPixel(x, y) == m_crTrans)
					x++;
				//记住这个起始点
				int iLeftX = x;
				//寻找下个透明色的点
				while (x < m_csBitmaps[0].dwWidth && dcMem.GetPixel(x, y) != m_crTrans)
					x++;
				//创建一个包含起点与重点间高为1像素的临时“region”
				rgnTemp.CreateRectRgn(iLeftX, y, x, y+1);
				//合并到主"region".
				wndRgn.CombineRgn(&wndRgn, &rgnTemp, RGN_OR);
				//删除临时"region",否则下次创建时和出错
				rgnTemp.DeleteObject();
			} 
		}
		// Set the button region
		SetWindowRgn(wndRgn,TRUE);
	}// if
} // End of SizeToContent

BOOL CBitButtonNL::SetBitmaps(UINT nBitmapNormal, UINT nBitmapMouseDown, UINT nBitmapHigh, UINT nBitmapDisable, COLORREF crTrans)
{
	// Set transparent color
	m_crTrans = crTrans;

	ASSERT(nBitmapNormal);

	HBITMAP		hBitmapNormal		= NULL;
	HBITMAP		hBitmapMouseDown	= NULL;
	HBITMAP		hBitmapHigh			= NULL;
	HBITMAP		hBitmapDisable		= NULL;
	HINSTANCE	hInstResource		= NULL;
	
	// Find correct resource handle
	hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(nBitmapNormal), RT_BITMAP);

	// Load bitmap Normal
	hBitmapNormal = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(nBitmapNormal), IMAGE_BITMAP, 0, 0, 0);

	// Load bitmap MouseDown
	if (nBitmapMouseDown != NULL)
			hBitmapMouseDown = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(nBitmapMouseDown), IMAGE_BITMAP, 0, 0, 0);
	// Load bitmap High
	if (nBitmapHigh != NULL)
			hBitmapHigh = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(nBitmapHigh), IMAGE_BITMAP, 0, 0, 0);
	// Load bitmap Disable
	if (nBitmapDisable != NULL)
			hBitmapDisable = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(nBitmapDisable), IMAGE_BITMAP, 0, 0, 0);

	return SetBitmaps(hBitmapNormal, hBitmapMouseDown, hBitmapHigh, hBitmapDisable, crTrans);
} // End of SetBitmaps

BOOL  CBitButtonNL::SetBitmaps(HBITMAP hBitmapNormal, HBITMAP hBitmapMouseDown, HBITMAP hBitmapHigh, HBITMAP hBitmapDisable, COLORREF crTrans)
{
	int		nRetValue = 0;
	BITMAP	csBitmapSize;

	// Set transparent color
	m_crTrans = crTrans;


	// Free any loaded resource
	FreeResources();

	if (hBitmapNormal)
	{
		m_csBitmaps[0].hBitmap = hBitmapNormal;
		// Get bitmap size
		nRetValue = ::GetObject(hBitmapNormal, sizeof(csBitmapSize), &csBitmapSize);
		if (nRetValue == 0)
		{
			FreeResources();
			return FALSE;
		} // if
		m_csBitmaps[0].dwWidth = (DWORD)csBitmapSize.bmWidth;
		m_csBitmaps[0].dwHeight = (DWORD)csBitmapSize.bmHeight;

		if (hBitmapMouseDown)
		{
			m_csBitmaps[1].hBitmap = hBitmapMouseDown;
			// Get bitmap size
			nRetValue = ::GetObject(hBitmapMouseDown, sizeof(csBitmapSize), &csBitmapSize);
			if (nRetValue == 0)
			{
				FreeResources();
				return FALSE;
			} // if
			m_csBitmaps[1].dwWidth = (DWORD)csBitmapSize.bmWidth;
			m_csBitmaps[1].dwHeight = (DWORD)csBitmapSize.bmHeight;

		} // if

		if (hBitmapHigh)
		{
			m_csBitmaps[2].hBitmap = hBitmapHigh;
			// Get bitmap size
			nRetValue = ::GetObject(hBitmapHigh, sizeof(csBitmapSize), &csBitmapSize);
			if (nRetValue == 0)
			{
				FreeResources();
				return FALSE;
			} // if
			m_csBitmaps[2].dwWidth = (DWORD)csBitmapSize.bmWidth;
			m_csBitmaps[2].dwHeight = (DWORD)csBitmapSize.bmHeight;

		} // if

		if (hBitmapDisable)
		{
			m_csBitmaps[3].hBitmap = hBitmapDisable;
			// Get bitmap size
			nRetValue = ::GetObject(hBitmapDisable, sizeof(csBitmapSize), &csBitmapSize);
			if (nRetValue == 0)
			{
				FreeResources();
				return FALSE;
			} // if
			m_csBitmaps[3].dwWidth = (DWORD)csBitmapSize.bmWidth;
			m_csBitmaps[3].dwHeight = (DWORD)csBitmapSize.bmHeight;

		} // if
	} // if

	Invalidate();

	return TRUE;
} // End of SetBitmaps

void CBitButtonNL::CancelHover(void)
{
	if (m_bMouseOnButton)
	{
		m_bMouseOnButton = FALSE;
		Invalidate();
	} // if
} // End of CancelHover

void CBitButtonNL::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd*				wndUnderMouse = NULL;
	CWnd*				wndActive = this;
	TRACKMOUSEEVENT		csTME;

	CButton::OnMouseMove(nFlags, point);

	ClientToScreen(&point);
	wndUnderMouse = WindowFromPoint(point);

	// If the mouse enter the button with the left button pressed then do nothing
	if (nFlags & MK_LBUTTON && m_bMouseOnButton == FALSE) return;

	if (wndUnderMouse && wndUnderMouse->m_hWnd == m_hWnd && wndActive)
	{
		if (!m_bMouseOnButton)
		{
			m_bMouseOnButton = TRUE;

			Invalidate();

#ifdef	BTNNL_USE_SOUND
			// Play sound ?
			if (m_csSounds[0].lpszSound)
				::PlaySound(m_csSounds[0].lpszSound, m_csSounds[0].hMod, m_csSounds[0].dwFlags);
#endif

			csTME.cbSize = sizeof(csTME);
			csTME.dwFlags = TME_LEAVE;
			csTME.hwndTrack = m_hWnd;
			::_TrackMouseEvent(&csTME);
		} // if
	} else CancelHover();
}

LRESULT CBitButtonNL::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	CancelHover();
	return 0;
} // End of OnMouseLeave

BOOL CBitButtonNL::OnClicked() 
{	
	SetFocus();

#ifdef	BTNNL_USE_SOUND
	// Play sound ?
	if (m_csSounds[1].lpszSound)
		::PlaySound(m_csSounds[1].lpszSound, m_csSounds[1].hMod, m_csSounds[1].dwFlags);
#endif

	return FALSE;
} // End of OnClicked

#ifdef	BTNNL_USE_SOUND
BOOL CBitButtonNL::SetSound(LPCTSTR lpszSound, HMODULE hMod, BOOL bPlayOnClick, BOOL bPlayAsync)
{
	BYTE	byIndex = bPlayOnClick ? 1 : 0;

	// Store new sound
	if (lpszSound)
	{
		if (hMod)	// From resource identifier ?
		{
			m_csSounds[byIndex].lpszSound = lpszSound;
		} // if
		else
		{
			_tcscpy(m_csSounds[byIndex].szSound, lpszSound);
			m_csSounds[byIndex].lpszSound = m_csSounds[byIndex].szSound;
		} // else

		m_csSounds[byIndex].hMod = hMod;
		m_csSounds[byIndex].dwFlags = SND_NODEFAULT | SND_NOWAIT;
		m_csSounds[byIndex].dwFlags |= hMod ? SND_RESOURCE : SND_FILENAME;
		m_csSounds[byIndex].dwFlags |= bPlayAsync ? SND_ASYNC : SND_SYNC;
	} // if
	else
	{
		// Or remove any existing
		::ZeroMemory(&m_csSounds[byIndex], sizeof(STRUCT_SOUND));
	} // else

	return TRUE;
} // End of SetSound
#endif

BOOL CBitButtonNL::SetForceColor(COLORREF crColorNormal, COLORREF crColorMouseDown, COLORREF crColorHigh, BOOL bRepaint)
{
	// Set new color
	m_crForceColors[BTNNL_COLOR_FG_NORMAL] = crColorNormal;
	m_crForceColors[BTNNL_COLOR_FG_CLICK] = crColorMouseDown;
	m_crForceColors[BTNNL_COLOR_FG_HIGH] = crColorHigh;

	if (bRepaint)	Invalidate();

	return TRUE;
} // End of SetColor

BOOL CBitButtonNL::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor != NULL)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	} // if

	return CButton::OnSetCursor(pWnd, nHitTest, message);
}

void CBitButtonNL::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
	CancelHover();
}

BOOL CBitButtonNL::SetBtnCursor(int nCursorId, BOOL bRepaint)
{
	HINSTANCE	hInstResource = NULL;
	// Destroy any previous cursor
	if (m_hCursor)
	{
		::DestroyCursor(m_hCursor);
		m_hCursor = NULL;
	} // if

	// Load cursor
	if (nCursorId)
	{
		hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(nCursorId), RT_GROUP_CURSOR);
		// Load cursor resource
		m_hCursor = (HCURSOR)::LoadImage(hInstResource, MAKEINTRESOURCE(nCursorId), IMAGE_CURSOR, 0, 0, 0);
		// Repaint the button
		if (bRepaint) Invalidate();
		// If something wrong
		if (m_hCursor == NULL) return FALSE;
	} // if

	return TRUE;
} // 

BOOL CBitButtonNL::SetDrawFocusRect(BOOL bDrawFlatFocus, BOOL bRepaint)
{
	m_bDrawFlatFocus = bDrawFlatFocus;
	// Repaint the button
	if (bRepaint) Invalidate();

	return TRUE;
} // End of DrawFlatFocus

BOOL CBitButtonNL::SetFocusRectDeflateRect(int x, int y)
{
	m_iDeflateRect_x = x;
	m_iDeflateRect_y = y;
	
	return TRUE;
}

void CBitButtonNL::ReconstructFont(void)
{
	m_font.DeleteObject();

	BOOL bCreated = m_font.CreateFontIndirect(&m_lf);
	ASSERT(bCreated);
}

CBitButtonNL& CBitButtonNL::SetFontBold(BOOL bBold, BOOL bRepaint)
{	
	m_lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;

	ReconstructFont();

	if (bRepaint) Invalidate();

	return *this;
}

CBitButtonNL& CBitButtonNL::SetFontName(const CString& strFont, BYTE byCharSet /* Default = ANSI_CHARSET */, BOOL bRepaint)
{	
	m_lf.lfCharSet = byCharSet;

	_tcscpy(m_lf.lfFaceName, strFont);
	
	ReconstructFont();

	if (bRepaint) Invalidate();

	return *this;
}

CBitButtonNL& CBitButtonNL::SetDrawText(BOOL bDraw, BOOL bRepaint)
{
	m_bDrawText = bDraw;

	if (bRepaint) Invalidate();

	return *this;
}

CBitButtonNL& CBitButtonNL::SetFontUnderline(BOOL bSet, BOOL bRepaint)
{
	m_lf.lfUnderline = bSet;
	ReconstructFont();

	if (bRepaint) Invalidate();

	return *this;
}

CBitButtonNL& CBitButtonNL::SetFontItalic(BOOL bSet, BOOL bRepaint)
{
	m_lf.lfItalic = bSet;
	ReconstructFont();

	if (bRepaint) Invalidate();

	return *this;	
}

CBitButtonNL& CBitButtonNL::SetFontSize(int nSize, BOOL bRepaint)
{
	CFont cf;
	LOGFONT lf;

	cf.CreatePointFont(nSize * 10, m_lf.lfFaceName);
	cf.GetLogFont(&lf);

	m_lf.lfHeight = lf.lfHeight;
	m_lf.lfWidth  = lf.lfWidth;

//	nSize*=-1;
//	m_lf.lfHeight = nSize;
	ReconstructFont();

	if (bRepaint) Invalidate();

	return *this;
}

CBitButtonNL& CBitButtonNL::SetFont(LOGFONT lf, BOOL bRepaint)
{
	CopyMemory(&m_lf, &lf, sizeof(m_lf));

	ReconstructFont();

	if (bRepaint) Invalidate();

	return *this;
}

CBitButtonNL& CBitButtonNL::SetFont3D(BOOL bSet, int i3dx, int i3dy, BOOL bRepaint)
{
	m_bFont3d = bSet;
	m_iText3d_x = i3dx;
	m_iText3d_y = i3dy;

	if (bRepaint) Invalidate();

	return *this;
}

CBitButtonNL& CBitButtonNL::SetText3DBKColor(COLORREF cr3DBKColor, BOOL bRepaint)
{
	m_cr3DBKColor = cr3DBKColor;

	if (bRepaint) Invalidate();

	return *this;
}

BOOL CBitButtonNL::SetTextAlign(BYTE byAlign, BOOL bRepaint)
{
	switch (byAlign)
	{    
		case NL_ALIGN_LEFT:
		case NL_ALIGN_RIGHT:
		case NL_ALIGN_CENTER:
			m_byTextAlign = byAlign;
			if (bRepaint)	Invalidate();
			return TRUE;
	} // switch

	return FALSE;
} // End of SetAlign

CBitButtonNL&  CBitButtonNL::SetDrawTextAsDisable(BOOL bSet, BOOL bRepaint)
{
	m_bDrawDisableText = bSet;

	if (bRepaint) Invalidate();

	return *this;
}

BOOL CBitButtonNL::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
//	return CButton::OnEraseBkgnd(pDC);
}

void CBitButtonNL::setChecked(bool bChecked)
{
	m_bChecked = bChecked;
	Invalidate();
}
