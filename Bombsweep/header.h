#pragma once
#include <windows.h>
#include <windowsx.h>																								//调用HANDLE_MSG宏
                                                                                                                    //需要使用GET_X_LPARAM和GET_Y_LPARAM
#include <time.h>																									//需要使用time()
#include <strsafe.h>																								//需要使用安全字符串函数
#include "resource.h"
#include "global.h"




LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HMENU fnCreateMenuBar();

void fnRestartGame(HWND, HDC, int, int);

void fnInitBrush();

void fnInitCMInfo();

int fnClientToRom(int, int);

BOOL fnCheckMousePos(int, int, int, int);

void fnInitLoadResource(HWND, HDC, int, int);

void fnRestoreCMInfo(int, int, int, int, int, int);

void fnDrawChessManual(HWND, HDC, int, int, int);

void fnSelectNeighborGap(int);

void fnOnCheckMenuItem(HMENU, int);

void fnRedraw(HWND, HDC);

void fnDrawFigures(HDC, int);

void fnRedrawFlag(HWND, HDC);

void fnRandomBomb(int, int);

void fnRedrawExpBomb(HDC);

void fnFiguresBound(int);

void fnCalculFigures();

void fnExposureBomb(HDC);

void fnDrawBomb(HDC, int, int, int, int);

int fnRoundingUp(double);

void fnDrawWrongBomb(HDC, int, int, int, int);

void fnDrawFlag(HWND, HDC, int);

void fnCheckWin(HWND, HDC);
