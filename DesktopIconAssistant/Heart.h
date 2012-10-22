#pragma once

class CHeart
{
public:
	CHeart(void);
	~CHeart(void);
	void setRect(LPRECT lpRect);
	void getPoint(double ratio, LPPOINT lpPoint);
	void draw(CDC& dc);
private:
	RECT m_rctHeart;
public:
	double getCircumference(void);
};
