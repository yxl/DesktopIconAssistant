#include "StdAfx.h"
#include "global.h"
#include "DesktopIcon.h"
#include "OS.h"

int g_nThread = 0;

// For the parameter of IconSwingThread
typedef struct tagIST
{
	CDesktopIcon *pDI;
	int		index;
} IST;

static UINT IconSwingThread(LPVOID pParam)
{
	const int N = 14;
	const int aSwingPos[N] = {1, 1, 2, 1, -1, -2, -2, -2, -1, -1, 1, 1, 1, 1};

	g_nThread++;

	IST *pIst = (IST*)pParam;
	CDesktopIcon *pDI = pIst->pDI;
	int index = pIst->index;
	delete pIst;

	POINT pos;	// initial position

	for (int i=0; i<N; i++)
	{
		pDI->getIconPosition(index, &pos);
		pDI->moveIcon(index, pos.x + aSwingPos[i], pos.y);
		Sleep(30);
	}

	g_nThread--;

	return 0;
}

CDesktopIcon::CDesktopIcon(void)
: m_lcIcon()
, m_iIconWidth(0)
, m_iIconHeight(0)
, m_nIcon(0)
, m_iUndoIndex(-1)
, m_vctUndo()
, m_aIconText(NULL)
{
}

CDesktopIcon::~CDesktopIcon(void)
{
	// Wait for IconSwingThread
	for (int i=0; i<10; i++)
	{
		if (g_nThread == 0)
			break;
		Sleep(100);
	}

	if (m_aIconText)
		delete[] m_aIconText;
	// Detach the icon list control
	m_lcIcon.Detach();
}

BOOL CDesktopIcon::init(void)
{
	RECT rect;
	DWORD dwProcessId;
	HANDLE hProcess;

	// Get the desktop window handle
	HWND hWndDesktop = NULL;
	hWndDesktop = FindWindow(_T("Progman"), NULL);
	hWndDesktop = FindWindowEx(hWndDesktop, NULL, _T("SHELLDLL_DefView"), NULL);

	// Get the desktop icon list control handle
	HWND hWnd = FindWindowEx(hWndDesktop, NULL, _T("SysListView32"), NULL);
	if (NULL == hWnd)
	{
		return FALSE;
	}

	m_lcIcon.Detach();
	// Attach the handle
	if (FALSE == m_lcIcon.Attach(hWnd))
	{
		return FALSE;
	}

	// Open the desktop icon process
	GetWindowThreadProcessId(m_lcIcon.m_hWnd, &dwProcessId);
	hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwProcessId);

	// Get the number of icons
	m_nIcon = m_lcIcon.GetItemCount();

	int t = m_lcIcon.GetHotItem();

	m_aIconText = new CString[m_nIcon];

	// Get icon size
	if (m_nIcon > 0)
	{
		LPRECT lpTmp = (LPRECT)VirtualAllocEx(hProcess, NULL, sizeof(RECT), MEM_COMMIT, PAGE_READWRITE);
		rect.left = LVIR_BOUNDS;
		WriteProcessMemory(hProcess, lpTmp, &rect, sizeof(RECT), NULL);
		SendMessage(m_lcIcon.m_hWnd, LVM_GETITEMRECT, 0, (LPARAM)lpTmp);
		ReadProcessMemory(hProcess, lpTmp, &rect, sizeof(RECT), NULL);
		VirtualFreeEx(hProcess, lpTmp, 0, MEM_RELEASE);   

		m_iIconHeight = rect.bottom - rect.top;
		//m_iIconWidth = rect.right - rect.left;
		m_iIconWidth = m_iIconHeight;
	}
	else
	{
		m_iIconWidth = 0;
		m_iIconHeight = 0;
	}

	
	// Allocate memory
	LPLVITEM lpItem = (LPLVITEM)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE);
	TCHAR *lpText = (TCHAR *)VirtualAllocEx(hProcess, NULL, sizeof(TCHAR) * MAX_TEXT_LEN, MEM_COMMIT, PAGE_READWRITE);

	// Initialize memory
	LVITEM item;
	item.iSubItem = 0;
	item.pszText = lpText;
	item.cchTextMax = MAX_TEXT_LEN;
	WriteProcessMemory(hProcess, lpItem, &item, sizeof(LVITEM), NULL);

	int n = 0;
	TCHAR szBuffer[MAX_TEXT_LEN];
	for (int i=0; i<m_nIcon; i++)
	{
		n = SendMessage(m_lcIcon.m_hWnd, LVM_GETITEMTEXT, i,(LPARAM)lpItem);
		ReadProcessMemory(hProcess, lpItem, &item, sizeof(RECT), NULL);
		ReadProcessMemory(hProcess, lpText, szBuffer, sizeof(TCHAR) * (n+1), NULL);
		m_aIconText[i] = szBuffer;
	}

	// Free the memory
	VirtualFreeEx(hProcess, lpItem, 0, MEM_RELEASE);   
	VirtualFreeEx(hProcess, lpText, 0, MEM_RELEASE);   

	// Close desktop icon process
	CloseHandle(hProcess);   

	if (Utils::OS::GetVersion() >= Utils::OS::VISTA)
	{
		setupDesktop(hWndDesktop);
	}

	return TRUE;
}

// Note: It does not work for WinXP. We have tested on Win7, but not on Vista.
void CDesktopIcon::setupDesktop(HWND hWndDesktop)
{
	static const WPARAM wRefresh = 0x7103;
	static const WPARAM wToggleAutoArrangeIcons = 0x7071;
	static const WPARAM wToggleAlignIconsToGrid = 0x7074;

	bool bIsAutoArrangeIconsEnabled = false;
	bool bIsAlginIconsToGridEnabled = false;
	bool bChanged = false;

	// 刷新桌面，更新注册表中的设置
	::SendMessage(hWndDesktop, WM_COMMAND, wRefresh, 0);

	// 从注册表读取“自动排列”和“对齐到网格”选项状态
	HKEY hkey;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\Shell\\Bags\\1\\Desktop"), 0,
		KEY_QUERY_VALUE, &hkey))
	{
		DWORD dwData = 0;
		DWORD dwSize = sizeof(dwData);

		if (ERROR_SUCCESS == RegQueryValueEx(hkey, _T("FFlags"), NULL, NULL, (LPBYTE)&dwData, &dwSize))
		{
			if (dwData & 0x01)
			{
				bIsAutoArrangeIconsEnabled = true;
			}
			if (dwData & 0x04)
			{

				bIsAlginIconsToGridEnabled = true;
			}
		}

		RegCloseKey(hkey);
	}

	// 取消“自动排列”选项
	if (bIsAutoArrangeIconsEnabled)
	{
		::SendMessage(hWndDesktop, WM_COMMAND, wToggleAutoArrangeIcons, 0);
		bChanged = true;
	}

	// 取消“对齐到网格”选项
	if (bIsAlginIconsToGridEnabled)
	{
		::SendMessage(hWndDesktop, WM_COMMAND, wToggleAlignIconsToGrid, 0);
		bChanged = true;
	}

	// 刷新桌面，将修改后设置的保存至注册表
	if (bChanged)
	{
		::SendMessage(hWndDesktop, WM_COMMAND, wRefresh, 0);
	}
}

int CDesktopIcon::getIconCount(void)
{
	return m_nIcon;
}


void CDesktopIcon::moveIcon(int index, long x, long y)
{
	ListView_SetItemPosition(m_lcIcon.m_hWnd, index, (int)(x + MINSPACE), (int)y);
}

int CDesktopIcon::getSelectedIconCount(void)
{
	int n = 0;
	n = m_lcIcon.GetSelectedCount();
	return n;
}

void CDesktopIcon::getSelectedIconIndices(int *aIndices)
{
	int index = 0;
	POSITION pos = m_lcIcon.GetFirstSelectedItemPosition();
	while (pos)
	{
		aIndices[index++] = m_lcIcon.GetNextSelectedItem(pos);
	}
}


void CDesktopIcon::dragIcon(int index, long dx, long dy)
{
	POINT ptCursor;		// initial mouse position
	POINT ptIcon;		// icon's initial position
	POINT ptCenter;		// icon's center coordinates
	
	
	GetCursorPos(&ptCursor); // get the initial mouse position

	// Get the screen coordinates of the center of the icon
	getIconPosition(index, &ptIcon);
	ptCenter.x = ptIcon.x + m_iIconWidth / 2;
	ptCenter.y = ptIcon.y + m_iIconHeight / 2;
	m_lcIcon.ClientToScreen(&ptCenter);

	// Select the icon
	SetCursorPos(ptCenter.x, ptCenter.y); 
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);	// left button down
	Sleep(200);

	// Move the icon
	mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, 0);	// mouse moving
	Sleep(50);

	// Drop the icon
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);	// left button up
	Sleep(200);

	// Restore the mouse position
	SetCursorPos(ptCursor.x, ptCursor.y);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);	// left button down
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);	// left button up
}

void CDesktopIcon::getIconPosition(int index, LPPOINT lpPoint)
{
	DWORD dwProcessId;
	HANDLE hProcess;
	GetWindowThreadProcessId(m_lcIcon.m_hWnd, &dwProcessId);
	hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwProcessId);
	LPPOINT lpTmp = (LPPOINT)VirtualAllocEx(hProcess, NULL, sizeof(POINT), MEM_COMMIT, PAGE_READWRITE);
    m_lcIcon.GetItemPosition(index, lpTmp);
	ReadProcessMemory(hProcess, lpTmp, lpPoint, sizeof(POINT), NULL);
	VirtualFreeEx(hProcess, lpTmp, 0, MEM_RELEASE);   
    CloseHandle(hProcess);   
	lpPoint->x -= MINSPACE;
}

int CDesktopIcon::getIconWidth(void)
{
	return m_iIconWidth;                                                                                                                                         
}

int CDesktopIcon::getIconHeight(void)
{
	return m_iIconHeight;
}


void CDesktopIcon::align(const int *aIndices, int n, int dir, int space)
{
	RECT rctDesktop;	// Coordinates of desktop area
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rctDesktop, 0);

	if (0 == space)
		space = m_iIconWidth / 2;


	int w = rctDesktop.right - rctDesktop.left;		// desktop width
	int h = rctDesktop.bottom - rctDesktop.top;		// desktop height
	int nRow = (h + space) / (m_iIconHeight + space);	// number of rows
	int nCol = (w + space) / (m_iIconWidth + space);	// maximum number of columns
	int num = min(n, nCol * nRow);	// number of icons to be aligned	
	int index = 0;	// index of the current icon
	int x, y;
	int row, col;
	if (LEFT == dir || RIGHT == dir)
	{
		for (col=0; col<nCol; col++)
		{
			for (row=0; row<nRow; row++)
			{
				x = MINSPACE + col * (m_iIconWidth + space);
				if (dir == RIGHT)
				{
					x = rctDesktop.right - x - m_iIconWidth + MINSPACE;
				}
				y = row * (m_iIconHeight + space);
				moveIcon(aIndices[index], x, y);
				// Whether aligns all the icons
				if (++index == num)
				{
					return;
				}
			} //for (row=0; row<nRow; row++)
		} // for (col=0; col<nCol; col++)
	} // if (LEFT == dir || RIGHT == dir)
	else if (UP == dir || DOWN == dir)
	{
		for (row=0; row<nRow; row++)
		{
			for (col=0; col<nCol; col++)
			{
				x = MINSPACE + col * (m_iIconWidth + space);
				y = row * (m_iIconHeight + space);
				if (DOWN == dir)
				{
					y = rctDesktop.bottom - y - m_iIconHeight;
				}
				moveIcon(aIndices[index], x, y);
				// Whether aligns all the icons
				if (++index == num)
				{
					return;
				}
			} // for (col=0; col<nCol; col++)
		} //for (row=0; row<nRow; row++)
	} // else if 
}

int CDesktopIcon::line(const int *aIndices, int n, int space, POINT ptBegin, POINT ptEnd)
{
	if (n <= 0)
		return 0;

	double dx = ptEnd.x - ptBegin.x;
	double dy = ptEnd.y - ptBegin.y;
	double length = sqrt(dx * dx + dy * dy);	// length of the line
	double dblIconSpace;						// the space between icons

	// Calculate the space automatically if necessary
	if (0 != space) 
	{
		dblIconSpace = space;
	}
	else
	{
		if (n > 1)
			dblIconSpace = max(MINSPACE, length / (n - 1) - m_iIconWidth);	// distribute the icons evenly
		else
			dblIconSpace = MINSPACE;
	}
	
	int num = min(n, (int)(length / (m_iIconWidth + dblIconSpace) + 1 + 0.5));	// number of icons to be arranged

	// direction vector
	double vx = dx / (length + EPS);
	double vy = dy / (length + EPS);

	// move the icons
	long x,y;
	for (int i=0; i<num; i++)
	{
		x = (long)(ptBegin.x + vx * i * (m_iIconWidth + dblIconSpace));
		y = (long)(ptBegin.y + vy * i * (m_iIconWidth + dblIconSpace));
		moveIcon(aIndices[i], x - m_iIconWidth/2, y - m_iIconWidth/2);
	}

	return num;
}

int CDesktopIcon::oval(const int *aIndices, int n, int space, POINT ptBegin, POINT ptEnd)
{
	if (n <= 0)
		return 0;

	// center coordinates
	POINT center;
	center.x = (ptBegin.x + ptEnd.x) / 2;
	center.y = (ptBegin.y + ptEnd.y) / 2;

	// radius
	long rx = labs(ptEnd.x - ptBegin.x) / 2;
	long ry = labs(ptEnd.y - ptBegin.y) / 2;
	long r = min(rx, ry);	// the radius of the biggest circle inside the oval

	double dblIconSpace;		// the space between icons

	// circumference of the biggest circle inside the oval
	double c = 2 * PI * r;	

	// Calculate the space automatically if necessary
	if (0 != space) 
	{
		dblIconSpace = space;
	}
	else
	{
		dblIconSpace = max(MINSPACE, c / n - m_iIconWidth);	// distribute the icons evenly
	}
	
	int num = min(n, (int)(c / (m_iIconWidth + dblIconSpace)));	// number of icons to be arranged

	long x,y;
	double angle = (m_iIconWidth + dblIconSpace) / (r + EPS);
	// move the icons
	for (int i=0; i<num; i++)
	{
		x = (long)(center.x + rx * cos(-i * angle));
		y = (long)(center.y + ry * sin(-i * angle));
		moveIcon(aIndices[i], x - m_iIconWidth/2, y - m_iIconWidth/2);
	}

	return num;
}

int CDesktopIcon::hollowRectangle(const int *aIndices, int n, int space, POINT ptBegin, POINT ptEnd)
{
	if (n <= 0)
		return 0;

	long w = labs(ptEnd.x - ptBegin.x);		// width
	long h = labs(ptEnd.y - ptBegin.y);		// height
	long c = 2 * (w + h);					// circumference 
	double dblIconSpace;					// the space between icons

	// Calculate the space automatically if necessary
	if (0 != space) 
	{
		dblIconSpace = space;
	}
	else
	{
		if (n > 1)
			dblIconSpace = max(MINSPACE, c / n - m_iIconWidth);	// distribute the icons evenly
		else 
			dblIconSpace = MINSPACE;
	}
	
	// Get the row and column numbers
	int col = (int)(w / (m_iIconWidth + dblIconSpace) + 1 + 0.5);
	int row = (int)(h / (m_iIconWidth + dblIconSpace) + 1 + 0.5);

	// Adjusted column space
	long colSpace;
	if (col > 1)
		colSpace = w / (col - 1) - m_iIconWidth;	
	else
		colSpace = (long)dblIconSpace;

	// Adjusted row space
	long rowSpace;
	if (row > 1)
		rowSpace = h / (row - 1) - m_iIconHeight;	
	else
		rowSpace = (long)dblIconSpace;

	int num = 0;	// number of moved icons

	long x,y;

	long left = min(ptBegin.x, ptEnd.x);
	long top = min(ptBegin.y, ptEnd.y);

	// arrange the left column
	for (int i=0; i<row; i++)
	{
		if (num >= n)
			goto endArrange;
		x = left;
		y = top + (m_iIconWidth + rowSpace) * i;
		moveIcon(aIndices[num++], x - m_iIconWidth/2, y - m_iIconWidth/2);
	}

	// arrange the top row
	for (int i=1; i<col; i++)
	{
		if (num >= n)
			goto endArrange;
		x = left + 	(m_iIconWidth + colSpace) * i;
		y = top;
		moveIcon(aIndices[num++], x - m_iIconWidth/2, y - m_iIconWidth/2);
	}

	// arrange the bottom row
	for (int i=1; i<col; i++)
	{
		if (num >= n)
			goto endArrange;
		x = left + 	(m_iIconWidth + colSpace) * i;
		y = top + (row - 1) * (m_iIconWidth + rowSpace);
		moveIcon(aIndices[num++], x - m_iIconWidth/2, y - m_iIconWidth/2);
	}


	// arrange the right column
	for (int i=1; i<row-1; i++)
	{
		if (num >= n)
			goto endArrange;
		x = left + (col - 1) * (m_iIconWidth + colSpace);
		y = top + (m_iIconWidth + rowSpace) * i;
		moveIcon(aIndices[num++], x - m_iIconWidth/2, y - m_iIconWidth/2);
	}
endArrange:

	return num;
}

int CDesktopIcon::fillRectangle(const int *aIndices, int n, int space, POINT ptBegin, POINT ptEnd)
{
	if (n <= 0)
		return 0;
	long w = labs(ptEnd.x - ptBegin.x);		// width
	long h = labs(ptEnd.y - ptBegin.y);		// height
	double dblIconSpace;					// the space between icons

	// Calculate the space automatically if necessary
	if (0 != space) 
	{
		dblIconSpace = space;
	}
	else
	{
		dblIconSpace = max(MINSPACE, sqrt((double)((w + m_iIconWidth) * (h + m_iIconHeight)) / n) - m_iIconWidth);	// distribute the icons evenly
	}

	int nRow = (int)(h / (m_iIconWidth + dblIconSpace) + 1 + 0.5);	// maximum number of rows

	int nCol;	// maximum number of columns
	if (0 != space)
		nCol = (int)(w / (m_iIconHeight + dblIconSpace) + 1 + 0.5);
	else
		nCol = (int)(min((double)n / (double)nRow + 0.5, w / (m_iIconHeight + dblIconSpace) + 1 + 0.5));	// maximum number of columns

	int num = min(n, nRow * nCol); // number of icons to be aligned	

	// Adjusted column space
	long colSpace;
	if (nCol > 1)
		colSpace = w / (nCol - 1) - m_iIconWidth;	
	else
		colSpace = (long)(dblIconSpace);

	// Adjusted row space
	long rowSpace;
	if (nRow > 1)
		rowSpace = h / (nRow - 1) - m_iIconHeight;	
	else
		rowSpace = (long)(dblIconSpace);

	long left = min(ptBegin.x, ptEnd.x);
	long top = min(ptBegin.y, ptEnd.y);

	int index = 0;	// index of the current icon
	long x, y;
	int row, col;
	for (col=0; col<nCol; col++)
	{
		for (row=0; row<nRow; row++)
		{
			x = left + col * (m_iIconWidth + colSpace);
			y = top + row * (m_iIconHeight + rowSpace);
			moveIcon(aIndices[index], x - m_iIconWidth/2, y - m_iIconWidth/2);
			// Whether move all the icons
			if (++index >= num)
				return num;
		}
	} 
	return 0;
}

int CDesktopIcon::heart(const int *aIndices, int n, int space, POINT ptBegin, POINT ptEnd)
{
	if (n <= 0)
		return 0;

	RECT rct;
	rct.left = min(ptBegin.x, ptEnd.x);
	rct.top = min(ptBegin.y, ptEnd.y);
	rct.right = max(ptBegin.x, ptEnd.x);
	rct.bottom = max(ptBegin.y, ptEnd.y);
	m_heart.setRect(&rct);

	double c = m_heart.getCircumference();		// Get the circumference
	double dblIconSpace;					// the space between icons

	// Calculate the space automatically if necessary
	if (0 != space) 
	{
		dblIconSpace = space;
	}
	else
	{
		dblIconSpace = max(MINSPACE, c / n - m_iIconWidth);	// distribute the icons evenly
	}

	int num = (int)min(n, c / (dblIconSpace + m_iIconWidth)); // number of icons to be aligned

	
	double ratio = (dblIconSpace + m_iIconWidth) / c;
	POINT pos;

	// move the icons
	for (int i=0; i<num; i++)
	{
		m_heart.getPoint(ratio * i, &pos);
		moveIcon(aIndices[i], pos.x - m_iIconWidth/2, pos.y - m_iIconWidth/2);
	}

	return num;
}

int CDesktopIcon::curve(const int *aIndices, int n, int space, const POINT *aPoints, int cntPoint)
{
	if (n <= 0)
		return 0;
	if (cntPoint < 2)
		return 0;

	double dx = 0, dy = 0;
	double length = 0;	// length of the curve
	double dblIconSpace; // the space between icons

	// Calculate the length of the curve
	for (int i=1; i<cntPoint; i++)
	{
		dx = aPoints[i].x - aPoints[i-1].x;
		dy = aPoints[i].y - aPoints[i-1].y;
		length += sqrt(dx * dx + dy * dy);
	}

	// Calculate the space automatically if necessary
	if (0 != space) 
	{
		dblIconSpace = space;
	}
	else
	{
		if (n > 1)
			dblIconSpace = max(MINSPACE, length / (n - 1) - m_iIconWidth);	// distribute the icons evenly
		else
			dblIconSpace = MINSPACE;
	}
	
	int num = min(n, (int)(length / (m_iIconWidth + dblIconSpace) + 1 + 0.5));	// number of icons to be arranged

	double len;
	double vx, vy;
	long x, y;
	int index = 0;
	double dist = 0;
	for (int i=1; i<cntPoint; i++)
	{
		dx = aPoints[i].x - aPoints[i-1].x;
		dy = aPoints[i].y - aPoints[i-1].y;
		len = sqrt(dx * dx + dy * dy);

		while (dist <= len)
		{
			// direction vector
			vx = dx / (len + EPS);
			vy = dy / (len + EPS);

			// move the icons
			x = (long)(aPoints[i-1].x + vx * dist);
			y = (long)(aPoints[i-1].y + vy * dist);
			moveIcon(aIndices[index++], x - m_iIconWidth/2, y - m_iIconWidth/2);
			dist += dblIconSpace + m_iIconWidth;
			if (index == num)
				break;
		}
		if (index == num)
			break;
		dist -= len;
	}
	if (index == num-1)
	{
		// move the last icons
		x = (long)(aPoints[index].x + vx * dist);
		y = (long)(aPoints[index].y + vy * dist);
		moveIcon(aIndices[index], x - m_iIconWidth/2, y - m_iIconWidth/2);
	}

	return num;
}

int CDesktopIcon::bezier(const int *aIndices, int n, int space, const POINT *aPoints)
{
	const int N = 100;
	POINT aCurvePoints[N];
	long a0, a1, a2, a3, b0, b1, b2, b3;
	double t;

	a0 = aPoints[0].x;
	a1 = -3 * aPoints[0].x + 3 * aPoints[1].x;
	a2 = 3 * aPoints[0].x - 6 * aPoints[1].x + 3 * aPoints[2].x;
	a3 = -aPoints[0].x + 3 * aPoints[1].x - 3 * aPoints[2].x + aPoints[3].x;

	b0 = aPoints[0].y;
	b1 = -3 * aPoints[0].y + 3 * aPoints[1].y;
	b2 = 3 * aPoints[0].y - 6 * aPoints[1].y + 3 * aPoints[2].y;
	b3 = -aPoints[0].y + 3 * aPoints[1].y - 3 * aPoints[2].y + aPoints[3].y;

	for (int i=0; i<N; i++)
	{
		t = i * 1.0 / (N-1); 
		aCurvePoints[i].x = (long) (a0 + a1*t + a2*t*t + a3*t*t*t); 
		aCurvePoints[i].y = (long) (b0 + b1*t + b2*t*t + b3*t*t*t);
	}
	return curve(aIndices, n, space, aCurvePoints, N);
}

BOOL CDesktopIcon::undo(void)
{	
	if (m_iUndoIndex < 1)
		return FALSE;

	m_iUndoIndex--;
	
	POINT pos = {0};
	for (int i=0; i<m_nIcon; i++)
	{
		pos = m_vctUndo.at(m_iUndoIndex * m_nIcon + i);
		moveIcon(i, pos.x, pos.y);
	}

	return TRUE;
}

BOOL CDesktopIcon::redo(void)
{
	if (m_iUndoIndex >= (int)(m_vctUndo.size() / m_nIcon - 1))
		return FALSE;
	
	m_iUndoIndex++;	

	POINT pos = {0};
	for (int i=0; i<m_nIcon; i++)
	{
		pos = m_vctUndo.at(m_iUndoIndex * m_nIcon + i);
		moveIcon(i, pos.x, pos.y);
	}

	return TRUE;
}

void CDesktopIcon::saveUndo(void)
{
	POINT pos = {0};

	// Ensure that the undos will less than MAX_UNDO_TIMES
	if (m_iUndoIndex >= MAX_UNDO_TIMES)
	{
		m_vctUndo.erase(m_vctUndo.begin(), m_vctUndo.begin() + (m_nIcon - 1));
		m_iUndoIndex--;
	}

	// Remove excessive redos
	if (m_iUndoIndex < (int)(m_vctUndo.size() / m_nIcon - 1))
	{
		m_vctUndo.erase(m_vctUndo.begin() + (m_iUndoIndex + 1) * m_nIcon, m_vctUndo.end());
	}

	// Save the undo.
	for (int i=0; i<m_nIcon; i++)
	{
		getIconPosition(i, &pos);
		m_vctUndo.push_back(pos);
	}

	m_iUndoIndex++;

}
void CDesktopIcon::getIconText(int index, TCHAR * szText)
{
	_tcscpy_s(szText, MAX_TEXT_LEN, m_aIconText[index]);
}

void CDesktopIcon::setIconText(int index, const TCHAR * szText)
{
	int n = _tcslen(szText);
	
	DWORD dwProcessId;
	HANDLE hProcess;

	GetWindowThreadProcessId(m_lcIcon.m_hWnd, &dwProcessId);
	hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwProcessId);

	// Allocate memory
	LPLVITEM lpItem = (LPLVITEM)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE);
	TCHAR *lpText = (TCHAR *)VirtualAllocEx(hProcess, NULL, sizeof(TCHAR) * (n + 1), MEM_COMMIT, PAGE_READWRITE);

	// Initilize memory
	LVITEM item;
	item.iSubItem = 0;
	item.pszText = lpText;
	WriteProcessMemory(hProcess, lpText, szText, 
		sizeof(TCHAR) * (n + 1), NULL);
	WriteProcessMemory(hProcess, lpItem, &item, sizeof(LVITEM), NULL);

	SendMessage(m_lcIcon.m_hWnd, LVM_SETITEMTEXT, index,(LPARAM)lpItem);
	
	// Free the memory
	VirtualFreeEx(hProcess, lpItem, 0, MEM_RELEASE);   
	VirtualFreeEx(hProcess, lpText, 0, MEM_RELEASE);   
	CloseHandle(hProcess);   
}

void CDesktopIcon::restoreIconText(int index)
{
	setIconText(index, m_aIconText[index]);
}

int CDesktopIcon::getHotIcon(void)
{
	return m_lcIcon.GetHotItem();
}

void CDesktopIcon::swing(int index)
{
	IST *pIst = new IST;
	pIst->index = index;
	pIst->pDI = this;
	AfxBeginThread(IconSwingThread, pIst);
}
