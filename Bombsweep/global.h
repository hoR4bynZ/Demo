#pragma once
#include "resource.h"

//-------------------------------------------------------------------------------------------------------定义一些标记
/*
 *fBoom：       						   该标记仅为0时，能调用fnRandomBomb()和fnCaculFigures()这些初始化函数。进行第一次初始化
 *fFigure：       						               该标记仅为0时，能在第一次调用fnRandomBomb()中调用fnFiguresBound()。
 *fLLock：         						                                   该标记表明之后鼠标左键点击不再响应————即游戏结束
 *fRLock：         	   					                                   该标记表明之后鼠标右键点击不再响应————即游戏结束
 *fFound：         						                                                      该标记表明当前剩余的雷数量
*/
static int fBoom, fFigure, fLLock, fRLock, fFound = 10;

//---------------------------------------------------------------------------------------------------保存格子的坐标信息
typedef struct tagRectangle {
	int pos;																										//标识该点位置
	int xLeft;																										//定义重绘矩形的左上角x坐标
	int yTop;																										//定义重绘矩形的左上角y坐标
	int xRight;																										//定义重绘矩形的右下角x坐标
	int yBottom;																									//定义重绘矩形的右下角y坐标
}Rect;
static Rect rect[625];



//--------------------------------------------------------------------------------------------保存棋谱是否需要绘制的标志
typedef struct tagChessManual {
	int boomflag;																									//标记是否存在雷，-1表示存在雷否则表示数字
	int drawflag;																									//标记是否需要重绘，0表示不用重绘否则需要
	int findflag;																									//标记是否已找到雷，1表示已找到
	int pos;																										//棋谱对应于一维数组的位置
}sCM;
static sCM szCm[625];																								//static限制了对外部文件不可见



//--------------------------------------------------------------------------------------------------------保存棋谱信息	
typedef struct tagChessManualInfo {
	int iSpecifications;																							//指明该棋谱的规格
	int iHeight;																									//总高度
	int iWidth;																										//总宽度
	int iBound;																										//棋谱边长
	int iBase;																										//每个格子边长
	int iBannerHg;																									//横额高度
	int iAmount;																									//棋谱格子的总数
	int iRandomRange;																								//生成的雷——随机数的范围
	int iBombAmount;																								//生成雷的数量
}sCMI;
static sCMI szCmi[3];



//------------------------------------------------------------------------------------------保存棋谱周围8格彼此间距的信息
typedef struct tagNeighborGap {
	int iRight;																										//距离右边第一位的距离
	int iRightUpper;																								//距离右上角第一位的距离
	int iTop;																										//距离上边第一位的距离
	int iLeftUpper;																									//距离左上角第一位的距离
	int iLeft;																										//距离左边第一位的距离
	int iLeftLower;																									//距离左下角第一位的距离
	int iBottom;																									//距离下边第一位的距离
	int iRightLower;																								//距离右下角第一位的距离
}sNG;
static sNG szNg[3];



HMENU hMenu;
HPEN hPen, hOldPen;
HBRUSH hBrush, hOldBrush;
LOGBRUSH lbLid, lbRemLid, lbExpBoom, lbBlack;																		//画盖子、去盖子、暴露雷和黑画刷的笔刷信息结构体
static int iBase, iUserChioce = PRIMARY;																			//每个格子的长度、用户选择的难度
static int cxChar, cyChar;																							//声明每个字符的宽度和高度
static int iRecentClick;																							//记录当前点击的位置
static int iAmount, iRandomRange, iBombAmount;																		//声明雷的总数量，随机雷的范围，以及雷的数量
static int itemp;																									//itp记录当前难度结构体信息数组的下标
static int iTimerCount;																								//记录时间