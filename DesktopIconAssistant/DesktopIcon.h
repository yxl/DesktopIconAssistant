#pragma once

#include "heart.h"
#include <vector>

using namespace std;

#define MINSPACE	(m_iIconWidth/8)
#define MAX_TEXT_LEN	256

class CDesktopIcon
{
public:
	CDesktopIcon(void);
	~CDesktopIcon(void);
	BOOL init(void);	
	int getIconCount(void);
	void moveIcon(int index, long x, long y);
	int getSelectedIconCount(void);
	void getSelectedIconIndices(int *aIndices);
	void dragIcon(int index, long dx, long dy);

	void getIconPosition(int index, LPPOINT lpPoint);
	int getIconWidth(void);
	int getIconHeight(void);

	void align(const int *aIndices, int n, int dir, int space);

	int line(const int *aIndices, int n, int space, POINT ptBegin, POINT ptEnd);
	int oval(const int *aIndices, int n, int space, POINT ptBegin, POINT ptEnd);
	int hollowRectangle(const int *aIndices, int n, int space, POINT ptBegin, POINT ptEnd);
	int fillRectangle(const int *aIndices, int n, int space, POINT ptBegin, POINT ptEnd);
	int heart(const int *aIndices, int n, int space, POINT ptBegin, POINT ptEnd);
	int curve(const int *aIndices, int n, int space, const POINT *aPoints, int cntPoint);
	int bezier(const int *aIndices, int n, int space, const POINT *aPoints);
	BOOL undo(void);
	BOOL redo(void);
	void saveUndo(void);

	void getIconText(int index, TCHAR * szText);
	void setIconText(int index, const TCHAR * szText);
	void restoreIconText(int index);
	int getHotIcon(void);
	void swing(int index);
private:
	void setupDesktop(HWND hWndDesktop);
	CListCtrl m_lcIcon;
	vector <POINT> m_vctUndo;
	int m_iIconWidth;
	int m_iIconHeight;
	int m_nIcon;
	int m_iUndoIndex;
	CHeart m_heart;
	CString *m_aIconText;
};
