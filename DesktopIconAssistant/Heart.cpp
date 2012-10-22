#include "StdAfx.h"
#include "global.h"
#include "Heart.h"

CHeart::CHeart(void)
{
	memset(&m_rctHeart, 0, sizeof(RECT));
}

CHeart::~CHeart(void)
{
}

void CHeart::setRect(LPRECT lpRect)
{
	m_rctHeart = *lpRect;
}


void CHeart::getPoint(double ratio, LPPOINT lpPoint)
{
	double w = m_rctHeart.right - m_rctHeart.left;
	double h = m_rctHeart.bottom - m_rctHeart.top;

	double ratioX = w / 8.0;
	double ratioY = h / (8.0 * 1.082);

	POINT O1, O2;

	O1.x = (long)(m_rctHeart.left + 2 * ratioX);
	O1.y = (long)(m_rctHeart.top + 3 * ratioY);
	
	O2.x = (long)(O1.x + 4 * ratioX);
	O2.y = (long)(O1.y);

	const double c = 13.7;
	const double ratioB1 = 2.2 / (c * 2);
	const double ratioC1 = (2.2 + 3.9) / (c * 2);
	const double ratioD = (2.2 + 3.9 + 7.6) / (c * 2);
	const double ratioC2 = (2.2 + 3.9 + 7.6 + 7.6) / (c * 2);
	const double ratioB2 = (2.2 + 3.9 + 7.6 + 7.6 + 3.9) / (c * 2);

	double r =  ratio - floor(ratio);	// Adjust the ratio to the range [0, 1]
	double angle;

	if (r < ratioB1)
	{
		angle = 48.19 / 180 * PI + r / ratioB1 * (90 - 48.19) / 180 * PI;
		lpPoint->x = (long)(O1.x + 3 * cos(angle) * ratioX);
		lpPoint->y = (long)(O1.y - 3 * sin(angle) * ratioY);
	}
	else if (r < ratioC1)
	{
		angle = (r - ratioB1) / (ratioC1 - ratioB1) * PI / 2; 
		lpPoint->x = (long)(O1.x - 2 * sin(angle) * ratioX);
		lpPoint->y = (long)(O1.y - 3 * cos(angle) * ratioY);
	}
	else if (r <ratioD)
	{
		angle = (r - ratioC1) / (ratioD - ratioC1) * 72.53 / 180 * PI;
		lpPoint->x = (long)(O2.x - 6 * cos(angle) * ratioX);
		lpPoint->y = (long)(O2.y + 6 * sin(angle) * ratioY);
	}
	else if (r < ratioC2)
	{
		angle = (ratioC2 - r) / (ratioC2 - ratioD) * 72.53 / 180 * PI;
		lpPoint->x = (long)(O1.x + 6 * cos(angle) * ratioX);
		lpPoint->y = (long)(O1.y + 6 * sin(angle) * ratioY);
	}
	else if (r < ratioB2)
	{
		angle = (ratioB2 - r) / (ratioB2 - ratioC2) * PI / 2; 
		lpPoint->x = (long)(O2.x + 2 * sin(angle) * ratioX);
		lpPoint->y = (long)(O2.y - 3 * cos(angle) * ratioY);
	}
	else
	{
		angle = 48.19 / 180 * PI + (1.0 - r) / (1 - ratioB2) * (90 - 48.19) / 180 * PI;
		lpPoint->x = (long)(O2.x - 3 * cos(angle) * ratioX);
		lpPoint->y = (long)(O2.y - 3 * sin(angle) * ratioY);
	}
}

void CHeart::draw(CDC& dc)
{
	double w = m_rctHeart.right - m_rctHeart.left;
	double h = m_rctHeart.bottom - m_rctHeart.top;

	double ratioX = w / 8.0;
	double ratioY = h / (8.0 * 1.082);

	POINT O1, O2, A, B1, B2, C1, C2, D;

	O1.x = (long)(m_rctHeart.left + 2 * ratioX);
	O1.y = (long)(m_rctHeart.top + 3 * ratioY);
	
	O2.x = (long)(O1.x + 4 * ratioX);
	O2.y = (long)(O1.y);

	A.x = (long)(O1.x + 2 * ratioX);
	A.y = (long)(m_rctHeart.top + ratioY * (3.0 - 2.236)); 

	B1.x = O1.x;
	B1.y = m_rctHeart.top;

	B2.x = O2.x;
	B2.y = B1.y;

	C1.x = m_rctHeart.left;
	C1.y = O1.y;
	
	C2.x = (long)(m_rctHeart.left + 8 * ratioX);
	C2.y = O1.y;

	D.x = A.x;
	D.y = (long)(O1.y + 5.657 * ratioY);

	RECT rect;
	long rx, ry;

	rx = (long)(3.0 * ratioX);
	ry = (long)(3.0 * ratioY);
	rect.left = O1.x - rx;
	rect.top = O1.y - ry;
	rect.right = O1.x + rx;
	rect.bottom = O1.y + ry;
	dc.Arc(&rect, A, B1);

	rect.left = O2.x - rx;
	rect.right = O2.x + rx;
	dc.Arc(&rect, B2, A);

	rx = (long)(2.0 * ratioX);
	ry = (long)(3.0 * ratioY);
	rect.left = O1.x - rx;
	rect.top = O1.y - ry;
	rect.right = O1.x + rx;
	rect.bottom = O1.y + ry;
	dc.Arc(&rect, B1, C1);

	rect.left = O2.x - rx;
	rect.right = O2.x + rx;
	dc.Arc(&rect, C2, B2);

	rx = (long)(6.0 * ratioX);
	ry = (long)(6.0 * ratioY);
	rect.left = O2.x - rx;
	rect.top = O2.y - ry;
	rect.right = O2.x + rx;
	rect.bottom = O2.y + ry;
	dc.Arc(&rect, C1, D);

	rect.left = O1.x - rx;
	rect.right = O1.x + rx;
	dc.Arc(&rect, D, C2);
}

double CHeart::getCircumference(void)
{
	double w = m_rctHeart.right - m_rctHeart.left;
	double ratioX = w / 8.0;
	return (2 * 13.7) * ratioX;
}
