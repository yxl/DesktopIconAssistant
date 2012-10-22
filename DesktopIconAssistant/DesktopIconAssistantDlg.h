// DesktopIconAssistantDlg.h : 头文件
// Created by Ben
// 2008-2-18

#pragma once
#include "global.h"
#include "bitbuttonnl.h"
#include "DesktopIcon.h"
#include "afxwin.h"
#include "afxcmn.h"


// CDesktopIconAssistantDlg 对话框
class CDesktopIconAssistantDlg : public CDialog
{
// 构造
public:
	CDesktopIconAssistantDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DESKTOPICONASSISTANT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);	// 设置ToolTip
	DECLARE_MESSAGE_MAP()

	virtual void OnOK();
	virtual void OnCancel();
public:
	///////////////////////////////////////////////////////////////
	// 鼠标消息

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);	// 左键拖动窗口
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);	
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);	// 左键双击最小化到托盘区

	///////////////////////////////////////////////////////////////
	// 按钮消息
	//

	// 系统按钮
	afx_msg void OnBnClickedButtonClose();	// 关闭
	afx_msg void OnBnClickedButtonMin();	// 最小化
	
	// 排列工具按钮
	afx_msg void OnBnClickedButtonLine();		// 直线工具
	afx_msg void OnBnClickedButtonHollowRect();	// 空心矩形工具
	afx_msg void OnBnClickedButtonFillRect();	// 实心矩形工具
	afx_msg void OnBnClickedButtonHeart();		// 心形工具 
	afx_msg void OnBnClickedButtonOval();		// 椭圆形工具
	afx_msg void OnBnClickedButtonBezier();		// Bezier曲线工具

	// 对齐工具按钮
	afx_msg void OnBnClickedButtonUp();			// 上
	afx_msg void OnBnClickedButtonDown();		// 下
	afx_msg void OnBnClickedButtonLeft();		// 左
	afx_msg void OnBnClickedButtonRight();		// 右

	// 间距选择按钮
	afx_msg void OnBnClickedButtonBig();		// 大
	afx_msg void OnBnClickedButtonMid();		// 中
	afx_msg void OnBnClickedButtonSmall();		// 小
	afx_msg void OnBnClickedButtonAuto();		// 自动
	afx_msg void OnBnClickedButtonCustom();		// 自定义
	
	// 其它按钮
	afx_msg void OnBnClickedButtonSave();		// 保存
	afx_msg void OnBnClickedButtonLoad();		// 读取
	afx_msg void OnBnClickedButtonUndo();		// 撤销
	afx_msg void OnBnClickedButtonRedo();		// 重复
	afx_msg void OnBnClickedButtonHideText();	// 隐藏桌面图标文字
	afx_msg void OnBnClickedButtonExchange();	// 交换两个图标的位置

	///////////////////////////////////////////////////////////////
	// 托盘菜单消息
	//

	// 系统菜单
	afx_msg void OnExit();			// 退出
	afx_msg void OnShow();			// 显示主界面
	afx_msg void OnAbout();			// 关于

	// 排列工具菜单	
	afx_msg void OnLine();			// 直线工具
	afx_msg void OnFillrect();		// 实心矩形工具
	afx_msg void OnHollowrect();	// 空心矩形工具
	afx_msg void OnHeart();			// 心形工具
	afx_msg void OnOval();			// 椭圆形工具
	afx_msg void OnBezier();		// Bezier曲线工具

	// 对齐工具菜单
	afx_msg void OnAlignUp();		// 上
	afx_msg void OnAlignDown();		// 下
	afx_msg void OnAlignLeft();		// 左
	afx_msg void OnAlignRight();	// 右

	// 间距选择菜单
	afx_msg void OnSpaceBig();		// 大
	afx_msg void OnSpaceMid();		// 中
	afx_msg void OnSpaceSmall();	// 小
	afx_msg void OnSpaceAuto();		// 自动
	afx_msg void OnSpaceCustom();	// 自定义

	// 其它菜单
	afx_msg void OnSave();			// 保存
	afx_msg void OnLoad();			// 读取
	afx_msg void OnRedo();			// 撤销
	afx_msg void OnUndo();			// 重复
	afx_msg void OnHideText();		// 隐藏桌面图标文字
	afx_msg void OnExchange();		// 交换两个图标的位置

	///////////////////////////////////////////////////////////////
	// 托盘图标消息
	//

	afx_msg HRESULT OnNotifyIcon(WPARAM wParam , LPARAM lParam);
public:
	///////////////////////////////////////////////////////////////
	// 控件变量
	//

	// 系统按钮
	CBitButtonNL m_btnClose;		// 关闭
	CBitButtonNL m_btnMin;			// 最小化

	// 排列工具菜单	
	CBitButtonNL m_btnLine;			// 直线工具	
	CBitButtonNL m_btnHollowRect;	// 空心矩形工具
	CBitButtonNL m_btnFillRect;		// 实心矩形工具
	CBitButtonNL m_btnHeart;		// 心形工具 
	CBitButtonNL m_btnOval;			// 椭圆形工具
	CBitButtonNL m_btnBezier;		// Bezier曲线工具


	// 对齐工具按钮
	CBitButtonNL m_btnUp;			// 上
	CBitButtonNL m_btnDown;			// 下
	CBitButtonNL m_btnLeft;			// 左
	CBitButtonNL m_btnRight;		// 右	
	
	// 间距选择按钮
	CBitButtonNL m_btnBig;			// 大
	CBitButtonNL m_btnMid;			// 中
	CBitButtonNL m_btnSmall;		// 小
	CBitButtonNL m_btnAuto;			// 自动
	CBitButtonNL m_btnCustom;		// 自定义

	// 其它按钮
	CBitButtonNL m_btnSave;			// 保存
	CBitButtonNL m_btnLoad;			// 读取
	CBitButtonNL m_btnUndo;			// 撤销
	CBitButtonNL m_btnRedo;			// 重复
	CBitButtonNL m_btnHideText;		// 隐藏桌面图标文字
	CBitButtonNL m_btnExchange;		// 交换两个图标的位置
	
	CToolTipCtrl m_toolTip;			// Tip控件
	
	// 自定义间距编辑框
	CEdit m_edtSpace;				
	int m_iEdtSpace;

private:
	CBitmap m_bmpBg;				// 背景图片
	BOOL m_bHideText;				// 是否隐藏桌面图标文字
	BOOL m_bShowTrayMenu;			// 是否显示托盘区菜单(使用排列工具的过程中不显示托盘区菜单)
	int *m_aIndices;				// 待操作的桌面图标的下标数组
	int m_n;						// 待操作的桌面图标的数目
	int m_iSpaceType;				// 图标的间距类型(可为SPACE_BIG, SPACE_MID, SPACE_SMALL, SPACE_AUTO, SPACE_CUSTOM)
	CDesktopIcon m_desktopIcon;		// 桌图图标控制变量
private:

	// 取图标间距的函数
	int getAlignSpace(void);		// 使用对齐工具时图标的间距(间距类型为自动时, 使用中等间距)
	int getSpace(void);				// 取间距值(间距类型为自动时，返回)

	void getIconIndices(void);		// 取得待操作的桌面图标(已选择的图标或所有图标)

	void align(int dir);			// 让桌面图标靠边对齐, dir为方向，可取 UP, DOWN, LEFT, RIGHT

	BOOL load(const TCHAR *szFileName);	// 保存桌面图标位置到文件
	BOOL save(const TCHAR *szFileName);	// 从文件读到桌面图标位置

	void showAllIconText(BOOL bShow);	// 显示/隐藏桌面图标文字
	
	void setFace(void);				// 初始化界面显示
	void setupRegion(void);			// 创建不规则窗口

	void addTrayIcon(void);			// 添加托盘图标
	void removeTrayIcon(void);		// 删除托盘图标

	void hideSpaceEdit(void);		// 隐藏自定义间距编辑框
	
	// 设定间距选择按钮的状态(将间距选择按钮模拟为一组单选框), index - 间距类型((可为SPACE_BIG, SPACE_MID, SPACE_SMALL, SPACE_AUTO, SPACE_CUSTOM))
	void setCheck(int index);		

	void displayDesktopDlg(int type);	// 显示排列工具的绘图窗口(类名CDesktopDlg)
public:
	void toggleDesktop(void);	// 发送Win+D快捷键消息, 显示/隐藏桌面

	// 使用排列工具排列桌面图标, 由排列工具的绘图窗口(类名CDesktopDlg)调用
	// 参数:
	//		ptBegin - 排列区域的起始点
	//		ptEnd	- 排列区域的终止点
	//		type	- 排列工具类型(可为LINE, FILLRECT, HOLLOWRECT, OVAL, HEART)
	void toolSort(POINT ptBegin, POINT ptEnd, int type);	
	
	void bezierSort(LPPOINT aPoits);

	int getSelectedIconCount(void); // 取得已选择的图标数, 由排列工具的绘图窗口(类名CDesktopDlg)调用
};
