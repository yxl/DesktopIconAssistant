//******************************************************************
// BitButtonNL.h : header file
// 名称：自定义热敏位图按钮 
// 功能：实现鼠标热敏，显示3d文字
// 说明：继承至CButton类
//
//									Author	   NingLi
//											2003-12-3
//******************************************************************

#if !defined(AFX_BITBUTTONNL_H__D6CCC57A_D357_41DA_ADC7_551F0726B3E8__INCLUDED_)
#define AFX_BITBUTTONNL_H__D6CCC57A_D357_41DA_ADC7_551F0726B3E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define	BTNNL_USE_SOUND

#define BTNNL_COLOR_FG_NORMAL	0
#define BTNNL_COLOR_FG_CLICK	1
#define BTNNL_COLOR_FG_HIGH		2

#define NL_ALIGN_LEFT			0
#define NL_ALIGN_RIGHT			1
#define NL_ALIGN_CENTER			2

/////////////////////////////////////////////////////////////////////////////
// CBitButtonNL window

class CBitButtonNL : public CButton
{
// Construction
public:

	CBitButtonNL();

// Attributes
public:
	
public:
//-------------------------------------------------------------------
//  函数：设置按钮位图
//	BOOL SetBitmaps(UINT nBitmapNormal,	UINT nBitmapMouseDown = NULL, UINT nBitmapHigh = NULL, UINT nBitmapDisable = NULL, COLORREF crTrans = RGB(0, 0, 0));
//  参数:
//	UINT nBitmapNormal		- 默认位图
//	UINT nBitmapMouseDown	- 鼠标按下 位图
//	UINT nBitmapHigh		- 鼠标移动到啊按钮上方 位图；
//	UINT nBitmapDisable		- 按钮无效 位图
//  COLORREF crTrans		- 透明色
//-------------------------------------------------------------------
	BOOL SetBitmaps(UINT nBitmapNormal, UINT nBitmapMouseDown = NULL, UINT nBitmapHigh = NULL, UINT nBitmapDisable = NULL, COLORREF crTrans = RGB(0, 0, 0));
	BOOL SetBitmaps(HBITMAP hBitmapNormal, HBITMAP hBitmapMouseDown = NULL, HBITMAP hBitmapHigh = NULL, HBITMAP hBitmapDisable = NULL, COLORREF crTrans = RGB(0, 0, 0));

//-------------------------------------------------------------------
//  函数：设置显示文字的色彩
//	BOOL SetForceColor(COLORREF crColorNormal, COLORREF crColorMouseDown, COLORREF crColorHigh, BOOL bRepaint = TRUE);
//  参数：
//	COLORREF crColorNormal		- 正常颜色
//	COLORREF crColorMouseDown	- 按下
//	COLORREF crColorHigh		- 热态
//	BOOL     bRepaint			- 是否立刻重画;
//-------------------------------------------------------------------
	BOOL SetForceColor(COLORREF crColorNormal, COLORREF crColorMouseDown, COLORREF crColorHigh, BOOL bRepaint = TRUE);

//-------------------------------------------------------------------
//  函数：设置是否显示焦点框
//	BOOL SetDrawFocusRect(BOOL bDrawFlatFocus, BOOL bRepaint = TRUE);
//  参数：
//	BOOL bDrawFlatFocus	- 显示焦点框 ?
//	BOOL bRepaint		- 是否立刻重画
//-------------------------------------------------------------------
	BOOL SetDrawFocusRect(BOOL bDrawFlatFocus, BOOL bRepaint = TRUE);

//-------------------------------------------------------------------
//  函数：设置焦点框与按钮边界的距离
//	BOOL SetFocusRectDeflateRect(int x, int y);
//  参数：
//	int x	- x 边距
//	int y	- y 边距
//-------------------------------------------------------------------
	BOOL SetFocusRectDeflateRect(int x, int y);

//-------------------------------------------------------------------
//  函数：设置热敏鼠标指针
//	BOOL SetBtnCursor(int nCursorId = NULL, BOOL bRepaint = TRUE);
//  参数：
//	int  nCursorId	- 指针
//	BOOL bRepaint	- 是否立刻重画
//-------------------------------------------------------------------
	BOOL SetBtnCursor(int nCursorId = NULL, BOOL bRepaint = TRUE);

//-------------------------------------------------------------------
//  函数：设置按钮文字排列方式
//	BOOL SetTextAlign(BYTE byAlign, BOOL bRepaint = TRUE);
//  参数：
//	BYTE byAlign  - 方式 有以下3种值
//					0. NL_ALIGN_LEFT
//					1. NL_ALIGN_RIGHT
//					2. NL_ALIGN_CENTER
//
//	BOOL bRepaint	- 是否立刻重画
//-------------------------------------------------------------------
	BOOL SetTextAlign(BYTE byAlign, BOOL bRepaint = TRUE);

//-------------------------------------------------------------------
//  函数：设置是否显示按钮文字
//	virtual CBitButtonNL& SetDrawText(BOOL bDraw = TRUE, BOOL bRepaint = TRUE);
//  参数：
//	BOOL bDraw  - 显示 ?
//	BOOL bRepaint	- 是否立刻重画
//-------------------------------------------------------------------
	virtual CBitButtonNL& SetDrawText(BOOL bDraw = TRUE, BOOL bRepaint = TRUE);

//-------------------------------------------------------------------
//  函数：设置按钮自适应第一个位图大小
//	void SizeToContent(void);
//  参数：
//	-
//-------------------------------------------------------------------
	void SizeToContent(void);

//-------------------------------------------------------------------
//  函数：设置字体是否为粗体
//	virtual CBitButtonNL& SetFontBold(BOOL bBold, BOOL bRepaint = TRUE);
//  参数：
//	BOOL bBold		- 粗体 ?
//	BOOL bRepaint	- 是否立刻重画
//-------------------------------------------------------------------
	virtual CBitButtonNL& SetFontBold(BOOL bBold, BOOL bRepaint = TRUE);

//-------------------------------------------------------------------
//  函数：设置字体名称
//	virtual CBitButtonNL& SetFontName(const CString& strFont, BYTE byCharSet = DEFAULT_CHARSET, BOOL bRepaint = TRUE);
//  参数：
//	const CString& strFont	- 字体名词
//	BYTE byCharSet			- 字符集
//	BOOL bRepaint			- 是否立刻重画
//-------------------------------------------------------------------
	virtual CBitButtonNL& SetFontName(const CString& strFont, BYTE byCharSet = DEFAULT_CHARSET, BOOL bRepaint = TRUE);

//-------------------------------------------------------------------
//  函数：设置字体下划线
//	virtual CBitButtonNL& SetFontUnderline(BOOL bSet, BOOL bRepaint = TRUE);
//  参数：
//	BOOL bSet		- 下划线 ?
//	BOOL bRepaint	- 是否立刻重画
//-------------------------------------------------------------------
	virtual CBitButtonNL& SetFontUnderline(BOOL bSet, BOOL bRepaint = TRUE);

//-------------------------------------------------------------------
//  函数：设置字体 斜体
//	virtual CBitButtonNL& SetFontItalic(BOOL bSet, BOOL bRepaint = TRUE);
//  参数：
//	BOOL bSet		- 斜体 ?
//	BOOL bRepaint	- 是否立刻重画
//-------------------------------------------------------------------
	virtual CBitButtonNL& SetFontItalic(BOOL bSet, BOOL bRepaint = TRUE);

//-------------------------------------------------------------------
//  函数：设置字体 字号
//	virtual CBitButtonNL& SetFontSize(int nSize, BOOL bRepaint = TRUE);
//  参数：
//	int nSize		- 字号
//	BOOL bRepaint	- 是否立刻重画
//-------------------------------------------------------------------
	virtual CBitButtonNL& SetFontSize(int nSize, BOOL bRepaint = TRUE);

//-------------------------------------------------------------------
//  函数：设置字体
//	virtual CBitButtonNL& SetFont(LOGFONT lf, BOOL bRepaint = TRUE);
//  参数：
//	LOGFONT lf		- 字体
//	BOOL bRepaint	- 是否立刻重画
//-------------------------------------------------------------------
	virtual CBitButtonNL& SetFont(LOGFONT lf, BOOL bRepaint = TRUE);

//-------------------------------------------------------------------
//  函数：设置 3d 字体 
//	virtual CBitButtonNL& SetFont3D(BOOL bSet, int i3dx = 3, int i3dy = 2, BOOL bRepaint = TRUE);
//  参数：
//	BOOL bSet		- 3d ?
//	int i3dx		- 阴影的距离 x
//	int i3dy		- 阴影的距离 y
//	BOOL bRepaint	- 是否立刻重画
//-------------------------------------------------------------------
	virtual CBitButtonNL& SetFont3D(BOOL bSet, int i3dx = 3, int i3dy = 2, BOOL bRepaint = TRUE);

//-------------------------------------------------------------------
//  函数：设置 3d 字体 阴影色彩
//	virtual CBitButtonNL& SetText3DBKColor(COLORREF cr3DBKColor, BOOL bRepaint = TRUE);
//  参数：
//	COLORREF cr3DBKColor - 阴影色彩
//	BOOL bRepaint		 - 是否立刻重画
//-------------------------------------------------------------------
	virtual CBitButtonNL& SetText3DBKColor(COLORREF cr3DBKColor, BOOL bRepaint = TRUE);	//Sets the 3D font hilite color

//-------------------------------------------------------------------
//  函数：设置 失效文字显示方式
//	virtual CBitButtonNL&  CBitButtonNL::SetDrawTextAsDisable(BOOL bSet, BOOL bRepaint = TRUE);
//  参数：
//	BOOL bSet			- True  = 自动画失效文字
//						  False = 按照设定色彩画文字
//	BOOL bRepaint		- 是否立刻重画
//-------------------------------------------------------------------
	virtual CBitButtonNL&  CBitButtonNL::SetDrawTextAsDisable(BOOL bSet, BOOL bRepaint = TRUE);

//-------------------------------------------------------------------
//  函数：设置 按钮声音
//	BOOL SetSound(LPCTSTR lpszSound, HMODULE hMod = NULL, BOOL bPlayOnClick = FALSE, BOOL bPlayAsync = TRUE);
//  参数：
//	LPCTSTR lpszSound		- 声音文件
//	HMODULE hMod			- 模式 资源 or 文件
//	BOOL bPlayOnClick		- 方式 True = 按下;		  False = 热态
//	BOOL bPlayAsync			- 模式 True = SND_ASYNC ; False = SND_SYNC
//-------------------------------------------------------------------

#ifdef	BTNNL_USE_SOUND
	BOOL SetSound(LPCTSTR lpszSound, HMODULE hMod = NULL, BOOL bPlayOnClick = FALSE, BOOL bPlayAsync = TRUE);
#endif

// Operations

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitButtonNL)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBitButtonNL();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBitButtonNL)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnClicked();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	virtual void DrawTheBitmap(CDC* pDC, BOOL bHasTitle, RECT* rpItem, CRect* rpCaption, BOOL bIsPressed, BOOL bIsDisabled);
	virtual void DrawTheText(CDC* pDC, LPCTSTR lpszText, RECT* rpItem, CRect* rpCaption, BOOL bIsPressed, BOOL bIsDisabled);
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	void FreeResources(BOOL bCheckForNULL = TRUE);
	void CancelHover(void);
	void ReconstructFont(void);

private:
	typedef struct _STRUCT_BITMAPS
	{
		HBITMAP		hBitmap;		// Handle to bitmap
		DWORD		dwWidth;		// Width of bitmap
		DWORD		dwHeight;		// Height of bitmap
	} STRUCT_BITMAPS;
#ifdef	BTNNL_USE_SOUND
#pragma pack(1)
	typedef struct _STRUCT_SOUND
	{
		TCHAR		szSound[_MAX_PATH];
		LPCTSTR		lpszSound;
		HMODULE		hMod;
		DWORD		dwFlags;
	} STRUCT_SOUND;
#pragma pack()

	STRUCT_SOUND	m_csSounds[2];	// Index 0 = Over	1 = Clicked
#endif

private:
	BOOL		m_bDrawFlatFocus;	// Draw focus rectangle for flat button?
	BOOL		m_bDrawText;		// Draw Text for button?
	BOOL		m_bMouseOnButton;	// Is mouse over the button?
	BOOL		m_bIsPressed;		// Is button pressed?
	BOOL		m_bIsFocused;		// Is button focused?
	BOOL		m_bIsDisabled;		// Is button disabled?
	BOOL		m_bIsDefault;		// Is default button?
	BOOL		m_bDrawDisableText;	// is Draw text as disable?
	int			m_iDeflateRect_x;	// DrawFlateFouse deflate rect _ x
	int			m_iDeflateRect_y;	// DrawFlateFouse deflate rect _ y
	POINT		m_ptPressedOffset;
	UINT		m_nTypeStyle;		// Button style
	HCURSOR		m_hCursor;			// Handle to cursor
	STRUCT_BITMAPS	m_csBitmaps[4];	// Button bitmap
	COLORREF	m_crForceColors[3];	// Colors to be used
	BYTE		m_byTextAlign;		// Align text
	COLORREF	m_crTrans;			// Transparent color
	BOOL		m_bChecked;			// Is checked?
	
	
	//Font
	LOGFONT		m_lf;
	CFont		m_font;
	BOOL		m_bFont3d;
	COLORREF	m_cr3DBKColor;
	int			m_iText3d_x, m_iText3d_y;

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void setChecked(bool bChecked);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITBUTTONNL_H__D6CCC57A_D357_41DA_ADC7_551F0726B3E8__INCLUDED_)
