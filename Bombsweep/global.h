#pragma once
#include "resource.h"

//-------------------------------------------------------------------------------------------------------����һЩ���
/*
 *fBoom��       						   �ñ�ǽ�Ϊ0ʱ���ܵ���fnRandomBomb()��fnCaculFigures()��Щ��ʼ�����������е�һ�γ�ʼ��
 *fFigure��       						               �ñ�ǽ�Ϊ0ʱ�����ڵ�һ�ε���fnRandomBomb()�е���fnFiguresBound()��
 *fLLock��         						                                   �ñ�Ǳ���֮�����������������Ӧ������������Ϸ����
 *fRLock��         	   					                                   �ñ�Ǳ���֮������Ҽ����������Ӧ������������Ϸ����
 *fFound��         						                                                      �ñ�Ǳ�����ǰʣ���������
*/
static int fBoom, fFigure, fLLock, fRLock, fFound = 10;

//---------------------------------------------------------------------------------------------------������ӵ�������Ϣ
typedef struct tagRectangle {
	int pos;																										//��ʶ�õ�λ��
	int xLeft;																										//�����ػ���ε����Ͻ�x����
	int yTop;																										//�����ػ���ε����Ͻ�y����
	int xRight;																										//�����ػ���ε����½�x����
	int yBottom;																									//�����ػ���ε����½�y����
}Rect;
static Rect rect[625];



//--------------------------------------------------------------------------------------------���������Ƿ���Ҫ���Ƶı�־
typedef struct tagChessManual {
	int boomflag;																									//����Ƿ�����ף�-1��ʾ�����׷����ʾ����
	int drawflag;																									//����Ƿ���Ҫ�ػ棬0��ʾ�����ػ������Ҫ
	int findflag;																									//����Ƿ����ҵ��ף�1��ʾ���ҵ�
	int pos;																										//���׶�Ӧ��һά�����λ��
}sCM;
static sCM szCm[625];																								//static�����˶��ⲿ�ļ����ɼ�



//--------------------------------------------------------------------------------------------------------����������Ϣ	
typedef struct tagChessManualInfo {
	int iSpecifications;																							//ָ�������׵Ĺ��
	int iHeight;																									//�ܸ߶�
	int iWidth;																										//�ܿ��
	int iBound;																										//���ױ߳�
	int iBase;																										//ÿ�����ӱ߳�
	int iBannerHg;																									//���߶�
	int iAmount;																									//���׸��ӵ�����
	int iRandomRange;																								//���ɵ��ס���������ķ�Χ
	int iBombAmount;																								//�����׵�����
}sCMI;
static sCMI szCmi[3];



//------------------------------------------------------------------------------------------����������Χ8��˴˼�����Ϣ
typedef struct tagNeighborGap {
	int iRight;																										//�����ұߵ�һλ�ľ���
	int iRightUpper;																								//�������Ͻǵ�һλ�ľ���
	int iTop;																										//�����ϱߵ�һλ�ľ���
	int iLeftUpper;																									//�������Ͻǵ�һλ�ľ���
	int iLeft;																										//������ߵ�һλ�ľ���
	int iLeftLower;																									//�������½ǵ�һλ�ľ���
	int iBottom;																									//�����±ߵ�һλ�ľ���
	int iRightLower;																								//�������½ǵ�һλ�ľ���
}sNG;
static sNG szNg[3];



HMENU hMenu;
HPEN hPen, hOldPen;
HBRUSH hBrush, hOldBrush;
LOGBRUSH lbLid, lbRemLid, lbExpBoom, lbBlack;																		//�����ӡ�ȥ���ӡ���¶�׺ͺڻ�ˢ�ı�ˢ��Ϣ�ṹ��
static int iBase, iUserChioce = PRIMARY;																			//ÿ�����ӵĳ��ȡ��û�ѡ����Ѷ�
static int cxChar, cyChar;																							//����ÿ���ַ��Ŀ�Ⱥ͸߶�
static int iRecentClick;																							//��¼��ǰ�����λ��
static int iAmount, iRandomRange, iBombAmount;																		//�����׵�������������׵ķ�Χ���Լ��׵�����
static int itemp;																									//itp��¼��ǰ�ѶȽṹ����Ϣ������±�
static int iTimerCount;																								//��¼ʱ��