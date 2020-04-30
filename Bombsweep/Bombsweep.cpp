//																													//29
#include "header.h"



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("WowBoom");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	BOOL bRet;

	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass)) {
		MessageBox(NULL, TEXT("这个程序需要在Windows NT才能执行！"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName,
		TEXT("MyWindows"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		316,
		439,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)    break;
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, hMemDc;
	HBITMAP hbmpMem, hPreBmpMem;
	PAINTSTRUCT ps;
	TEXTMETRIC tm;
	LPMINMAXINFO lpmmi;
	RECT rc;
	static int cxClient, cyClient;																					//客户区的长度和宽度
	int xPos, yPos, firstClick;																						//xPos, yPos为鼠标位置，firstClick为鼠标第一点位置
	TCHAR buffer[ARR_MAX];
	size_t iTarget;

	switch (message) {
	case WM_CREATE:
	{
		hdc = GetDC(hwnd);

		//----------------------------------------------------------------------------------------------初始化棋谱信息
		fnInitCMInfo();
		fnInitBrush();

		//-------------------------------------------------------------------------------------------------创建菜单栏
		hMenu = fnCreateMenuBar();
		SetMenu(hwnd, hMenu);

		//---------------------------------------------------------------------------------------获取字符宽度高度等信息
		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight + tm.tmExternalLeading;

		ReleaseDC(hwnd, hdc);
		return 0;
	}

	case WM_SIZE:
	{
		hdc = GetDC(hwnd);

		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		ReleaseDC(hwnd, hdc);
		return 0;
	}

	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rc);

		//------------------------------------------------------------------------------------------------创建双缓冲区
		hMemDc = CreateCompatibleDC(hdc);																			//创建内存DC
		if (!hMemDc) {
			MessageBox(hwnd, L"CreateCompatibleDC failed!", L"Failed", MB_OK);
			ReleaseDC(hwnd, hMemDc);
			DeleteDC(hMemDc);
			return 0;
		}

		hbmpMem = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);													//创建内存Bmp
		if (!hbmpMem) {
			MessageBox(hwnd, L"CreatecompatibleBitmap failed!", L"Failed", MB_OK);
			DeleteObject(hbmpMem);
			return 0;
		}

		hPreBmpMem = (HBITMAP)SelectObject(hMemDc, hbmpMem);														//绘图工具bmp选入设备DC

		//--------------------------------------------------------------------------------------编辑hbmpMem——绘制其背景
		hBrush = CreateSolidBrush(RGB(255, 255, 255));
		FillRect(hMemDc, &rc, hBrush);

		//---------------------------------------------------------------初始化内容：重新保存棋谱相关的信息，比如边长高度等等
		fnInitLoadResource(hwnd, hMemDc, cxClient, cyClient);

		//----------------------------------------------------------------------------------------------------重绘内容
		fnRedrawFlag(hwnd, hMemDc);
		fnRedraw(hwnd, hMemDc);

		//--------------------------------------------------------------fBoom为1表明已生成了雷，fLLock为1表示游戏胜利或失败
		//--------------------------------------------------------------------------fFound为INT_MAX时表明玩家已找到全部雷
		//-------------------------------------------------游戏因为胜利或失败已结束，从而锁定了左键，但需要在WM_PAINT消息中维持
		if (fBoom == 1 && fLLock == 1 && fFound != INT_MAX) {
			fnRedrawExpBomb(hMemDc);
			if (!BitBlt(
				hdc, 0, 0,
				rc.right, rc.bottom,
				hMemDc, 0, 0,
				SRCCOPY)
				) {
				MessageBox(hwnd, L"Bitblt failed!", L"Failed", MB_OK);
				ReleaseDC(hwnd, hMemDc);
				DeleteDC(hMemDc);
				DeleteObject(hbmpMem);
				EndPaint(hwnd, &ps);
				return 0;
			}
		}

		if (fFound == INT_MAX || fLLock == 1) {
			//----------------------------------------------------------------------------如果找到了所有的雷，或者扫错了雷

			//---------------------------------------------------------------------------------------------显示所用时间
			KillTimer(hwnd, IDT_TIMER);
			SetTextAlign(hMemDc, TA_RIGHT);
			StringCchPrintf(buffer, 100, TEXT("%d"), iTimerCount);
			StringCchLength(buffer, 100, &iTarget);
			TextOut(
				hMemDc,
				(cxClient + szCmi[itemp].iWidth) / 2 - 4 - szCmi[itemp].iBound / 15 - szCmi[itemp].iBound % 15 - szCmi[itemp].iBound / 12,
				(cyClient - szCmi[itemp].iHeight) / 2 + 4 + szCmi[itemp].iBannerHg / 4 + szCmi[itemp].iBound / 15,
				buffer,
				iTarget
			);

			//------------------------------------------------------------------------------------显示当前剩余雷的数目为0
			SetTextAlign(hMemDc, TA_LEFT);
			StringCchPrintf(buffer, 100, TEXT("%d"), 0);
			StringCchLength(buffer, 100, &iTarget);
			TextOut(
				hMemDc,
				(cxClient - szCmi[itemp].iWidth) / 2 + 4 + szCmi[itemp].iBound / 6,
				(cyClient - szCmi[itemp].iHeight) / 2 + 4 + szCmi[itemp].iBannerHg / 4 + szCmi[itemp].iBound / 15,
				buffer,
				iTarget
			);
		}
		else if (fFound != INT_MAX) {
			//----------------------------------------------------------------否则若在正常游戏中,显示正确的时间以及剩余雷数目
			SetTextAlign(hMemDc, TA_LEFT);
			StringCchPrintf(buffer, 100, TEXT("%d"), fFound);
			StringCchLength(buffer, 100, &iTarget);
			TextOut(
				hMemDc,
				(cxClient - szCmi[itemp].iWidth) / 2 + 4 + szCmi[itemp].iBound / 6,
				(cyClient - szCmi[itemp].iHeight) / 2 + 4 + szCmi[itemp].iBannerHg / 4 + szCmi[itemp].iBound / 15,
				buffer,
				iTarget
			);

			SetTextAlign(hMemDc, TA_RIGHT);
			StringCchPrintf(buffer, 100, TEXT("%d"), iTimerCount);
			StringCchLength(buffer, 100, &iTarget);
			TextOut(
				hMemDc,
				(cxClient + szCmi[itemp].iWidth) / 2 - 4 - szCmi[itemp].iBound / 15 - szCmi[itemp].iBound % 15 - szCmi[itemp].iBound / 12,
				(cyClient - szCmi[itemp].iHeight) / 2 + 4 + szCmi[itemp].iBannerHg / 4 + szCmi[itemp].iBound / 15,
				buffer,
				iTarget
			);
		}

		//-----------------------------------------------------------------------------------------------双缓冲传输位块
		if (!BitBlt(
			hdc, 0, 0,
			rc.right, rc.bottom,
			hMemDc, 0, 0,
			SRCCOPY)
			) {
			MessageBox(hwnd, L"Bitblt failed!", L"Failed", MB_OK);
			ReleaseDC(hwnd, hMemDc);
			DeleteDC(hMemDc);
			DeleteObject(hbmpMem);
			EndPaint(hwnd, &ps);
			return 0;
		}

		//---------------------------------------------------------------------------------释放双缓冲所用设备DC或绘图工具
		ReleaseDC(hwnd, hMemDc);
		DeleteDC(hMemDc);
		DeleteObject(hbmpMem);

		EndPaint(hwnd, &ps);
		return 0;
	}

	case WM_TIMER:
	{
		iTimerCount += 1;

		//----------------------------------------------------------------------------------------只重绘计时器存在的区域
		rc.left = (cxClient + szCmi[itemp].iWidth) / 2 - 4 - szCmi[itemp].iBound / 15 - szCmi[itemp].iBound % 15 - szCmi[itemp].iBound / 6;
		rc.top = (cyClient - szCmi[itemp].iHeight) / 2 + 4 + szCmi[itemp].iBannerHg / 4 + szCmi[itemp].iBound / 15;
		rc.right = (cxClient + szCmi[itemp].iWidth) / 2 - 4 - szCmi[itemp].iBound / 15 - szCmi[itemp].iBound % 15 - szCmi[itemp].iBound / 12;
		rc.bottom = (cyClient - szCmi[itemp].iHeight) / 2 + 4 + szCmi[itemp].iBannerHg / 4 + szCmi[itemp].iBound / 15 + cyChar;

		InvalidateRect(hwnd, &rc, FALSE);
		return 0;
	}

	case WM_GETMINMAXINFO:
	{
		//---------------------------------------------------------------------------------------------限制窗口最小尺寸
		lpmmi = (LPMINMAXINFO)lParam;
		lpmmi->ptMinTrackSize.x = 316;
		lpmmi->ptMinTrackSize.y = 439;
		return 0;
	}

	case WM_COMMAND:
		hdc = GetDC(hwnd);

		switch (LOWORD(wParam)) {
		case IDM_DEGREE_PRIMARY:																				//简单难度
		{
			//---------------------------------------------------需要先检查是否已在游戏中，因为不允许在游戏中随便改变难度
			//-----------------------------------------判断的条件是fBoom是否为1，因为fBoom为1表明游戏已经过初始化生成了雷
			if (fBoom != 1) {
				iUserChioce = PRIMARY;
				fnOnCheckMenuItem(hMenu, 0x0000);
				MoveWindow(hwnd, 80, 80, 311, 439, TRUE);
				fnDrawChessManual(hwnd, hdc, cxClient, cyClient, PRIMARY);
				fFound = szCmi[itemp].iBombAmount;																//修改剩余雷的数量
			}
			else {
				MessageBox(hwnd, TEXT("？？？"), TEXT("游戏中不允许改变难度！"), MB_OK);
			}
			break;
		}
		case IDM_DEGREE_INTERMEDIATE:																			//中等难度
		{
			if (fBoom != 1) {
				iUserChioce = INTERMEDIATE;
				fnOnCheckMenuItem(hMenu, 0x0001);
				MoveWindow(hwnd, 80, 80, 410, 589, TRUE);
				fnDrawChessManual(hwnd, hdc, cxClient, cyClient, INTERMEDIATE);
				fFound = szCmi[itemp].iBombAmount;																//修改剩余雷的数量
			}
			else {
				MessageBox(hwnd, TEXT("？？？"), TEXT("游戏中不允许改变难度！"), MB_OK);
			}
			break;
		}
		case IDM_DEGREE_SENIOR:																					//困难难度
		{
			if (fBoom != 1) {
				iUserChioce = SENIOR;
				fnOnCheckMenuItem(hMenu, 0x0002);
				MoveWindow(hwnd, 80, 80, 650, 909, TRUE);
				fnDrawChessManual(hwnd, hdc, cxClient, cyClient, SENIOR);
				fFound = szCmi[itemp].iBombAmount;																//修改剩余雷的数量
			}
			else {
				MessageBox(hwnd, TEXT("？？？"), TEXT("游戏中不允许改变难度！"), MB_OK);
			}
			break;
		}
		case IDM_GAME_BEGIN:
		{
			fnRestartGame(hwnd, hdc, cxClient, cyClient);

			//----------------------------------------------------------------------------------------初始化计时器
			iTimerCount = 0;
			KillTimer(hwnd, IDT_TIMER);

			break;
		}
		case IDM_GAME_EXIT:
		{
			SendMessage(hwnd, WM_DESTROY, wParam, lParam);
			break;
		}
		default:
			break;
		}

		//---------------------------------------------------------------------------------每次选择不同难度都重绘横额内容
		rc.left = (cxClient - szCmi[itemp].iWidth) / 2 + 4 + szCmi[itemp].iBound / 15;
		rc.top = (cyClient - szCmi[itemp].iHeight) / 2 + 4 + szCmi[itemp].iBannerHg / 4 + szCmi[itemp].iBound / 15;
		rc.right = (cxClient + szCmi[itemp].iWidth) / 2 - 4 - szCmi[itemp].iBound / 15;
		rc.bottom = (cyClient - szCmi[itemp].iHeight) / 2 + 4 + szCmi[itemp].iBannerHg / 4 + szCmi[itemp].iBound / 15 + cyChar;

		InvalidateRect(hwnd, &rc, FALSE);

		ReleaseDC(hwnd, hdc);
		return 0;

	case WM_LBUTTONDOWN:
		hdc = GetDC(hwnd);

		xPos = GET_X_LPARAM(lParam);
		yPos = GET_Y_LPARAM(lParam);

		GetClientRect(hwnd, &rc);

		//------------------------------------------------------------------------------------------------创建双缓冲区
		hMemDc = CreateCompatibleDC(hdc);																			//创建内存DC
		if (!hMemDc) {
			MessageBox(hwnd, L"CreateCompatibleDC failed!", L"Failed", MB_OK);
			DeleteDC(hMemDc);
			return 0;
		}

		hbmpMem = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);													//创建内存Bmp
		if (!hbmpMem) {
			MessageBox(hwnd, L"CreatecompatibleBitmap failed!", L"Failed", MB_OK);
			DeleteObject(hbmpMem);
			return 0;
		}

		//---------------------------------------------------------------------必须先检查鼠标的位置是否在不响应区域，右击同
		if (!fnCheckMousePos(cxClient, cyClient, xPos, yPos))    return 0;

		if (fBoom != 1) {																							//仅当fBoom不等于1时，表明还未生成雷
			//----------------------------------------------------------------------------------------------创建定时器
			SetTimer(hwnd, IDT_TIMER, 1000, (TIMERPROC)NULL);
			//----------------------------------------------------------------------------------------------进行初始化
			firstClick = fnClientToRom(xPos, yPos);
			//----------------------------------------------------------------------------------------------计算随机雷
			fnRandomBomb(firstClick, iUserChioce);
			//---------------------------------------------------------------------------------计算随即类周围8格子的数字
			fnCalculFigures();
			//---------------------------------------------------------------把fBoom设为1，即下次响应鼠标左键不再进行初始化
			fBoom = 1;
		}

		if (fLLock != 1) {																							//检查是否已经扫到雷，是的话不能再继续点击
			iRecentClick = fnClientToRom(xPos, yPos);

			if (szCm[iRecentClick].findflag != 1) {																	//如果点击的格子已经标记了棋子，就不能左击
				if (szCm[iRecentClick].boomflag != -1) {															//如果点击的格子不是雷
					//-----------------------------------------------------检查当前点击的位置，如果是数字0则暴露附近数字边界
					fnFiguresBound(iRecentClick);
				}
				else {																								//如果点击到雷
					//--------------------------------------------------------------------------------------暴露所有雷
					fnExposureBomb(hMemDc);
					fLLock = 1;																						//并且锁定棋谱
					fRLock = 1;
					MessageBox(hwnd, TEXT("你扫到雷了宝贝"), TEXT("You lose"), MB_OK);
				}
			}
		}

		ReleaseDC(hwnd, hMemDc);
		DeleteDC(hMemDc);
		DeleteObject(hbmpMem);
		ReleaseDC(hwnd, hdc);

		//----------------------------------------------------------------------------------------------仅绘制重绘部分
		for (int i = 0; i < szCmi[itemp].iSpecifications * szCmi[itemp].iSpecifications; i++) {
			if (szCm[i].drawflag) {
				rc.left = rect[i].xLeft;
				rc.top = rect[i].yTop;
				rc.right = rect[i].xRight;
				rc.bottom = rect[i].yBottom;

				InvalidateRect(hwnd, &rc, FALSE);
			}
		}

		return 0;

	case WM_RBUTTONDOWN:
		hdc = GetDC(hwnd);

		xPos = GET_X_LPARAM(lParam);
		yPos = GET_Y_LPARAM(lParam);

		GetClientRect(hwnd, &rc);

		//------------------------------------------------------------------------------------------------创建双缓冲区
		hMemDc = CreateCompatibleDC(hdc);																			//创建内存DC
		if (!hMemDc) {
			MessageBox(hwnd, L"CreateCompatibleDC failed!", L"Failed", MB_OK);
			DeleteDC(hMemDc);
			return 0;
		}

		hbmpMem = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);													//创建内存Bmp
		if (!hbmpMem) {
			MessageBox(hwnd, L"CreatecompatibleBitmap failed!", L"Failed", MB_OK);
			DeleteObject(hbmpMem);
			return 0;
		}

		if (!fnCheckMousePos(cxClient, cyClient, xPos, yPos))    return 0;

		//-------------------------------------------------------------------fRLock不为1即没找解锁全部雷，而且没有点击到雷
		//----------------------------------------------------------------fBoom为1表示已经经过了初始化————第一点不能是右击
		if (fRLock != 1 && fBoom == 1) {

			//----------------------------------------------------------------------------------初始化最近点击的坐标信息
			iRecentClick = fnClientToRom(xPos, yPos);

			if (szCm[iRecentClick].drawflag != 1) {																	//检查未揭开的格子

				//-------------------------------------------------检查当前游戏是否还存在雷，以及检查当前格子是否存在旗帜标记
				if (fFound <= 0 && szCm[iRecentClick].findflag == 0) {
					MessageBox(hwnd, TEXT("你必须取消一枚旗帜标记"), TEXT("你怎么肥事"), MB_OK);
				}
				else {//-----------------------------------------------------------------------------否则绘制旗帜的图案
					fnDrawFlag(hwnd, hMemDc, iRecentClick);
					fnCheckWin(hwnd, hMemDc);
				}
			}
		}

		ReleaseDC(hwnd, hMemDc);
		DeleteDC(hMemDc);
		DeleteObject(hbmpMem);
		ReleaseDC(hwnd, hdc);

		//---------------------------------------------------------------------------------------------重绘旗帜所在区域
		rc.left = rect[iRecentClick].xLeft;
		rc.top = rect[iRecentClick].yTop;
		rc.right = rect[iRecentClick].xRight;
		rc.bottom = rect[iRecentClick].yBottom;
		if (!InvalidateRect(hwnd, &rc, FALSE))  MessageBox(hwnd, L"RBOTTONDOWN redraw flag is failed to send WM_PAINT", L"", MB_OK);

		UpdateWindow(hwnd);

		//----------------------------------------------------------------------------------------重绘文字"标记"所在区域
		rc.left = (cxClient - szCmi[itemp].iWidth) / 2 + 4 + szCmi[itemp].iBound / 15;
		rc.top = (cyClient - szCmi[itemp].iHeight) / 2 + 4 + szCmi[itemp].iBannerHg / 4 + szCmi[itemp].iBound / 15;
		rc.right = (cxClient - szCmi[itemp].iWidth) / 2 + 4 + szCmi[itemp].iBound / 15 + szCmi[itemp].iBound / 4;
		rc.bottom = (cyClient - szCmi[itemp].iHeight) / 2 + 4 + szCmi[itemp].iBannerHg / 4 + szCmi[itemp].iBound / 15 + cyChar;
		if (!InvalidateRect(hwnd, &rc, FALSE))  MessageBox(hwnd, L"RBOTTONDOWN redraw char is failed to send WM_PAINT", L"", MB_OK);

		return 0;

	case WM_DESTROY:
		KillTimer(hwnd, IDT_TIMER);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

/*
 *功能：开始游戏
*/
void fnRestartGame(HWND hwnd, HDC hdc, int cxC, int cyC) {
	int i;

	//----------------------------------------------------------------------------------------------------初始化各标志
	fFound = szCmi[itemp].iBombAmount;
	fBoom = fFigure = fLLock = fRLock = 0;

	//---------------------------------------------------------------------------------------------初始化棋谱信息的标志
	for (i = 0; i < szCmi[itemp].iSpecifications * szCmi[itemp].iSpecifications; i++) {
		szCm[i].boomflag = szCm[i].drawflag = szCm[i].findflag = szCm[i].pos = 0;
	}

	fnDrawChessManual(hwnd, hdc, cxC, cyC, szCmi[itemp].iSpecifications);
}

/*
 *功能：初始化画刷的LOGBRUSH结构
*/
void fnInitBrush() {

	//------------------------------------------------------------------------------------------------------初始化画刷
	lbLid.lbStyle = BS_SOLID;
	lbLid.lbColor = RGB(54, 54, 54);

	lbRemLid.lbStyle = BS_SOLID;
	lbRemLid.lbColor = RGB(255, 255, 255);

	lbExpBoom.lbStyle = BS_SOLID;
	lbExpBoom.lbColor = RGB(255, 0, 0);

	lbBlack.lbStyle = BS_SOLID;
	lbBlack.lbColor = RGB(0, 0, 0);
}

/*
 *功能：客户区坐标转换成内存位置
*/
int fnClientToRom(int x, int y) {
	//------------------------------------------------------------------------声明行line，列column，相对于棋谱上的位置pos
	//-------------------------------------in是记录当前所在行，因为在for循环中判断行递增条件为每次加9——比如我当前点击位置是第二行
	//----------------------------------------------------------但在for循环中会导致当前行为9，所以需要一个另外的变量记录当前行
	int i, line, column, pos, in = 0;
	for (i = 0; i < szCmi[itemp].iSpecifications; i++) {
		if (x >= rect[i].xLeft && x <= rect[i].xRight) {															//x坐标在第一行格子的哪一个格子区间
			column = i;
			break;																									//已检索到目标行列便退出循环
		}
	}
	for (i = 0; i <= szCmi[itemp].iRandomRange; i += szCmi[itemp].iSpecifications) {
		if (y >= rect[i].yTop && y <= rect[i].yBottom) {
			line = in;
			break;
		}
		in++;
	}
	return pos = line * iUserChioce + column;
}

/*
 *功能：检查鼠标坐标是否为不响应区域
*/
BOOL fnCheckMousePos(int cxC, int cyC, int xP, int yP) {
	//-------------------------------------------------------------------------------------xy坐标必须位于棋谱内才给予响应
	if (
		(xP >= ((cxC - szCmi[itemp].iWidth) / 2 + 4)
			&& xP <= ((cxC + szCmi[itemp].iWidth) / 2 - 4)
			) &&
			(yP >= ((cyC - szCmi[itemp].iHeight) / 2 + szCmi[itemp].iBannerHg + 8)
				&& yP <= ((cyC + szCmi[itemp].iHeight) / 2 - 4)
				)
		)
		return TRUE;
	else return FALSE;
}


/*
 *功能：初始化加载
*/
void fnInitLoadResource(HWND hwnd, HDC hdc, int cxC, int cyC) {
	fnDrawChessManual(hwnd, hdc, cxC, cyC, iUserChioce);
}

/*
 *功能：保存棋谱信息
 *参数：iwh宽度，iht高度，ibd边界长度，ibht横幅高度，ibe每个格子边长，is棋谱规格
 *（备注：在生成图案时会用到这些信息）
*/
void fnRestoreCMInfo(int iwh, int iht, int ibd, int ibht, int ibe, int is) {
	int i;

	//------------------------------------------------------------------------根据参数is检查规格，判断属于哪个棋谱信息结构体
	for (i = 0; i < 3; i++) {
		if (is == szCmi[i].iSpecifications) {
			szCmi[i].iWidth = iwh;
			szCmi[i].iHeight = iht;
			szCmi[i].iBound = ibd;
			szCmi[i].iBannerHg = ibht;
			szCmi[i].iBase = ibe;
			break;
		}
	}
}

/*
 *功能：初始化棋谱信息 以及 初始化范围9以内格子的间隔信息
*/
void fnInitCMInfo() {

	//-----------------------------------------------------------------------------------------依次初始化棋谱信息结构体的
	//---------------------------每个难度下棋谱的规格、每个难度下棋谱格子的总数量、每个难度下棋谱生成雷的范围、每个难度下棋谱雷的数量
	szCmi[0].iSpecifications = PRIMARY;
	szCmi[0].iAmount = PRIMARY * PRIMARY;
	szCmi[0].iRandomRange = PRIMARY * PRIMARY - 9;
	szCmi[0].iBombAmount = 10;

	szCmi[1].iSpecifications = INTERMEDIATE;
	szCmi[1].iAmount = INTERMEDIATE * INTERMEDIATE;
	szCmi[1].iRandomRange = INTERMEDIATE * INTERMEDIATE - 9;
	szCmi[1].iBombAmount = 40;

	szCmi[2].iSpecifications = SENIOR;
	szCmi[2].iAmount = SENIOR * SENIOR;
	szCmi[2].iRandomRange = SENIOR * SENIOR - 9;
	szCmi[2].iBombAmount = 100;

	//--------------------------------------------------------------------------------------初始化范围9以内格子的间隔信息
	szNg[0].iRight = szNg[1].iRight = szNg[2].iRight = 1;
	szNg[0].iLeft = szNg[1].iLeft = szNg[2].iLeft = -1;

	szNg[0].iRightUpper = -8;
	szNg[0].iTop = -9;
	szNg[0].iLeftUpper = -10;
	szNg[0].iLeftLower = 8;
	szNg[0].iBottom = 9;
	szNg[0].iRightLower = 10;

	szNg[1].iRightUpper = -15;
	szNg[1].iTop = -16;
	szNg[1].iLeftUpper = -17;
	szNg[1].iLeftLower = 15;
	szNg[1].iBottom = 16;
	szNg[1].iRightLower = 17;

	szNg[2].iRightUpper = -24;
	szNg[2].iTop = -25;
	szNg[2].iLeftUpper = -26;
	szNg[2].iLeftLower = 24;
	szNg[2].iBottom = 25;
	szNg[2].iRightLower = 26;
}

/*
 *功能：绘制棋谱
 *参数：iBound表示可接受的边界
*/
void fnDrawChessManual(HWND hwnd, HDC hdc, int cxC, int cyC, int iBound) {
	int ih, iw, ib, i, j, count = 0;																				//声明高度ih和宽度iw、棋谱长度ib
	int iRemainder = 0;																								//计算每个格子的长度iBase因为整除会出现余数，这个余数会导致绘制末尾对不准
	int iDlgWd;																										//显示标记和时间的两个矩形的宽
	POINT sLT, sRB, sOrigin;																						//左上角和右下角的POINT结构以及起始点坐标											
	size_t iTarget;

	hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	hOldPen = (HPEN)SelectObject(hdc, hPen);
	hBrush = CreateSolidBrush(RGB(211, 211, 211));
	if (!hBrush)  MessageBox(hwnd, L"fnDrawChessManual画刷：？？？", L"老子卡了", MB_OK);
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	if (hOldBrush == NULL || hOldBrush == HGDI_ERROR)  MessageBox(hwnd, L"fnDrawChessManual SelectObject：???", L"老子卡了", MB_OK);

	if (4 * cxC >= 3 * cyC) {
		//condition A
		ih = cyC;
		iw = 3 * ih / 4;
		ib = iw - 8;

		iBase = (ib - iBound + 1) / iBound;																			//iBase是每个格子的长度
		iRemainder = (ib - iBound + 1) % iBound;

		sLT.x = (cxC - iw) / 2 + 4;
		sLT.y = 4;
		sRB.x = (cxC + iw) / 2 - 4 - iRemainder;
		sRB.y = ih - 8 - ib;

		sOrigin.x = (cxC - iw) / 2 + 4;
		sOrigin.y = ih - ib - 4;

		Rectangle(hdc, sLT.x, sLT.y, sRB.x, sRB.y);
		for (i = 0; i < iBound; i++) {
			for (j = 0; j < iBound; j++) {
				rect[count].xLeft = sOrigin.x + iBase * j + 1 * j;
				rect[count].yTop = sOrigin.y + iBase * i + 1 * i;
				rect[count].xRight = sOrigin.x + iBase * (j + 1) + 1 * j;
				rect[count].yBottom = sOrigin.y + iBase * (i + 1) + 1 * i;
				rect[count].pos = count;
				if (!szCm[count].drawflag) {
					//----------------------------------没有重绘标记的格子才需要绘制棋谱，因为有重绘标记的格子是覆盖在棋谱上面的
					Rectangle(hdc, rect[count].xLeft, rect[count].yTop, rect[count].xRight, rect[count].yBottom);
				}
				count++;
			}
		}

	}
	else {
		//condition B
		iw = cxC;
		ih = 4 * iw / 3;
		ib = iw - 8;

		iBase = (ib - iBound + 1) / iBound;																			//iBase是每个格子的长度
		iRemainder = (ib - iBound + 1) % iBound;

		sLT.x = 4;
		sLT.y = (cyC - ih) / 2 + 4;
		sRB.x = iw - 4 - iRemainder;
		sRB.y = (cyC + ih) / 2 - ib - 8;

		sOrigin.x = 4;
		sOrigin.y = (cyC + ih) / 2 - ib - 4;

		Rectangle(hdc, sLT.x, sLT.y, sRB.x, sRB.y);
		for (i = 0; i < iBound; i++) {
			for (j = 0; j < iBound; j++) {
				rect[count].xLeft = sOrigin.x + iBase * j + 1 * j;
				rect[count].yTop = sOrigin.y + iBase * i + 1 * i;
				rect[count].xRight = sOrigin.x + iBase * (j + 1) + 1 * j;
				rect[count].yBottom = sOrigin.y + iBase * (i + 1) + 1 * i;
				rect[count].pos = count;
				if (!szCm[count].drawflag) {
					Rectangle(hdc, rect[count].xLeft, rect[count].yTop, rect[count].xRight, rect[count].yBottom);
				}
				count++;
			}
		}

	}

	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);

	//--------------------------------------------------------------------------------------绘制两个显示时间和标记的矩形
	/*SelectObject(hdc, hOldPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	hOldPen = (HPEN)SelectObject(hdc, hPen);
	SelectObject(hdc, hOldBrush);
	hBrush = CreateSolidBrush(RGB(255, 255, 255));
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);*/

	iDlgWd = ib / 5;

	SetTextAlign(hdc, TA_LEFT);
	SetBkMode(hdc, TRANSPARENT);
	StringCchLength(TEXT("标记"), 100, &iTarget);
	TextOut(
		hdc,
		(cxC - iw) / 2 + 4 + ib / 15 + iDlgWd + ib / 25,
		(cyC - ih) / 2 + 4 + (ih - ib - 12) / 4 + iDlgWd / 3,
		TEXT("标记"),
		iTarget
	);

	SetTextAlign(hdc, TA_RIGHT);
	StringCchLength(TEXT("秒表"), 100, &iTarget);
	TextOut(
		hdc,
		(cxC + iw) / 2 - 4 - ib / 15 - ib % 15 - iDlgWd - ib / 25,
		(cyC - ih) / 2 + 4 + (ih - ib - 12) / 4 + iDlgWd / 3,
		TEXT("秒表"),
		iTarget
	);

	if (fBoom != 1) {
		//-----------------------------------------把棋谱的宽度，高度，边界，横额的高度，每个格子的边长以及用户选择的难度传入数组
		fnRestoreCMInfo(iw, ih, ib, ih - ib - 12, iBase, iUserChioce);
		//---------------------------------------------------------------------------------------------------选择间隙
		fnSelectNeighborGap(iUserChioce);
	}

}

/*
 *功能：根据用户的难度选择相应的范围9内格子的间隙
 *参数：iUserChioce
*/
void fnSelectNeighborGap(int fuc) {
	int i;

	//--------------------------------------------------------------------判断用户选择的难度，选择相应的结构体信息生成随机雷
	for (i = 0; i < 3; i++) {
		if (fuc == szCmi[i].iSpecifications) {																		//判断用户选择的难度
			iAmount = szCmi[i].iAmount;
			iRandomRange = szCmi[i].iRandomRange;
			iBombAmount = szCmi[i].iBombAmount;
			itemp = i;
			break;
		}
	}
}

/*
 *功能：创建菜单栏
*/
HMENU fnCreateMenuBar() {
	HMENU hMenu = CreateMenu();

	HMENU hGameMenu = CreateMenu();
	AppendMenu(hGameMenu, MF_STRING, IDM_GAME_BEGIN, TEXT("&开局"));
	AppendMenu(hGameMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hGameMenu, MF_STRING, IDM_GAME_EXIT, TEXT("&退出"));
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hGameMenu, TEXT("&菜单"));

	HMENU hDegreeMenu = CreateMenu();
	AppendMenu(hDegreeMenu, MF_STRING | MF_CHECKED, IDM_DEGREE_PRIMARY, TEXT("&简单"));
	AppendMenu(hDegreeMenu, MF_STRING, IDM_DEGREE_INTERMEDIATE, TEXT("&中等"));
	AppendMenu(hDegreeMenu, MF_STRING, IDM_DEGREE_SENIOR, TEXT("&困难"));
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hDegreeMenu, TEXT("&难度"));

	return hMenu;
}

/*
 *功能：生成随机的雷
 *执行该函数以后，第一点及周围8格不产雷，产雷的格子标记boomflag为-1
 */
void fnRandomBomb(int area, int fuc) {
	int count = 0, i, j, random;																					//count为计数器,random为随机数
	int szCheckRepeat[625] = { 0 };																					//该数组存放随机数

	//--------------------------------------------------------------------------------------------先用pos标记设置无雷区
	szCm[area].drawflag = 1;																						//同时将无雷的这个格子设置为重绘，下同
	szCm[area].pos = INT_MAX;																						//pos为无穷大表明这是一个无雷的格子，下同

	//-----------------------------------------------------------------------鼠标第一点的周围8格为无雷区，而且也需要检查边界
	if (
		(area + szNg[itemp].iRight) < iUserChioce * iUserChioce
		&& (area + szNg[itemp].iRight) >= 0
		)
	{
		if ((area + szNg[itemp].iRight) % iUserChioce != 0)
		{																											//检查右侧第一个格子
			szCm[area + szNg[itemp].iRight].pos = INT_MAX;
		}
	}

	if (
		(area + szNg[itemp].iRightUpper) < iUserChioce * iUserChioce
		&& (area + szNg[itemp].iRightUpper) >= 0
		)
	{
		if ((area + szNg[itemp].iRightUpper) % iUserChioce != 0)
		{																											//检查右上角第一个格子
			szCm[area + szNg[itemp].iRightUpper].pos = INT_MAX;
		}
	}

	if (
		(area + szNg[itemp].iTop) < iUserChioce * iUserChioce
		&& (area + szNg[itemp].iTop) >= 0)
	{																												//检查正上方第一个格子
		szCm[area + szNg[itemp].iTop].pos = INT_MAX;
	}

	if (
		(area + szNg[itemp].iLeftUpper) < iUserChioce * iUserChioce
		&& (area + szNg[itemp].iLeftUpper) >= 0
		)
	{
		if ((area + szNg[itemp].iLeftUpper) % iUserChioce != (iUserChioce - 1))
		{																											//检查左上角第一个格子
			szCm[area + szNg[itemp].iLeftUpper].pos = INT_MAX;
		}
	}

	if (
		(area + szNg[itemp].iLeft) < iUserChioce * iUserChioce
		&& (area + szNg[itemp].iLeft) >= 0
		)
	{
		if ((area + szNg[itemp].iLeft) % iUserChioce != (iUserChioce - 1))
		{																											//检查左侧第一个格子
			szCm[area + szNg[itemp].iLeft].pos = INT_MAX;
		}
	}

	if (
		(area + szNg[itemp].iLeftLower) < iUserChioce * iUserChioce
		&& (area + szNg[itemp].iLeftLower) >= 0
		)
	{
		if ((area + szNg[itemp].iLeftLower) % iUserChioce != (iUserChioce - 1))
		{																											//检查左下角第一个格子
			szCm[area + szNg[itemp].iLeftLower].pos = INT_MAX;
		}
	}

	if (
		(area + szNg[itemp].iBottom) < iUserChioce * iUserChioce
		&& (area + szNg[itemp].iBottom) >= 0
		)
	{																												//检查正下方第一个格子
		szCm[area + szNg[itemp].iBottom].pos = INT_MAX;
	}

	if (
		(area + szNg[itemp].iRightLower) < iUserChioce * iUserChioce
		&& (area + szNg[itemp].iRightLower) >= 0
		)
	{
		if ((area + szNg[itemp].iRightLower) % iUserChioce != 0)
		{																											//检查右下角第一个格子
			szCm[area + szNg[itemp].iRightLower].pos = INT_MAX;
		}
	}

	//-------------------------------------------------------------------------对二维数组pos不为INT_MAX的元素进行重新排序
	for (i = 0; i < iAmount; i++) {																					//将除无雷区以外的其他
		if (szCm[i].pos != INT_MAX) {																				//结构体元素按顺序排序
			szCm[i].pos = count;
			count++;
		}
	}

	//----------------------------------------------------------------------------产生n个随机数，产生的随机数为存在雷的格子
	srand((unsigned)(time(NULL)));
	szCheckRepeat[0] = rand() % iRandomRange;
	for (i = 0; i < iBombAmount; i++) {																				//每产生一个随机数先检查是否重复
		random = rand() % iRandomRange;																				//再存入数组
		for (j = 0; j < i; j++) {
			if (szCheckRepeat[j] != random) {																		//判断重复
				if (szCm[random].pos != INT_MAX) {
					szCheckRepeat[i] = random;
				}
				else {
					i--;
					break;
				}
			}
			else {
				i--;
				break;
			}
		}
	}

	//-----------------------------------------------------------------------将雷区的boomflag置为-1，其他元素该成员还原为0
	for (i = 0; i < iBombAmount; i++) {
		random = szCheckRepeat[i];																					//取出一个随机数
		for (j = 0; j < iAmount; j++) {																				//与全部81个数进行对比
			if (szCm[j].pos == random) {
				szCm[j].boomflag = -1;																				//随机数即雷
				break;
			}
		}
	}

	//--------------------------------------------------------------------将pos按顺序排好，在figureBound()中会用到pos标记
	for (i = 0; i < iAmount; i++) {
		szCm[i].pos = i;
	}

}

/*
 *功能：更改难度菜单栏的勾选状态
*/
void fnOnCheckMenuItem(HMENU hMenu, int ideg) {
	CheckMenuItem(hMenu, IDM_DEGREE_PRIMARY, MF_UNCHECKED);
	CheckMenuItem(hMenu, IDM_DEGREE_INTERMEDIATE, MF_UNCHECKED);
	CheckMenuItem(hMenu, IDM_DEGREE_SENIOR, MF_UNCHECKED);
	CheckMenuItem(hMenu, IDM_DEGREE_PRIMARY + ideg, MF_CHECKED);
}

/*
 *功能：重画格子或数字
 *将棋谱上drawflag为1的格子进行重绘
 */
void fnRedraw(HWND hwnd, HDC hdc) {
	int i;

	hBrush = CreateBrushIndirect(&lbRemLid);
	if (!hBrush)  MessageBox(hwnd, L"fnRedraw：???", L"老子卡了", MB_OK);
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	if (hOldBrush == NULL || hOldBrush == HGDI_ERROR)  MessageBox(hwnd, L"fnRedraw SelectObject：???", L"老子卡了", MB_OK);

	for (i = 0; i < szCmi[itemp].iSpecifications * szCmi[itemp].iSpecifications; i++) {
		if (szCm[i].drawflag) {																						//只有当雷的重绘标记为重绘时才进行重绘

			Rectangle(hdc, rect[i].xLeft, rect[i].yTop, rect[i].xRight, rect[i].yBottom);							//重绘格子

			fnDrawFigures(hdc, i);																					//重绘数字
		}
	}

	SelectObject(hdc, hOldBrush);
}

/*
 *功能：画数字
*/
void fnDrawFigures(HDC hdc, int i) {
	int ix, iy, xL, yT, xR, yB, iFigure;

	xL = rect[i].xLeft;
	yT = rect[i].yTop;
	xR = rect[i].xRight;
	yB = rect[i].yBottom;

	ix = (xR - xL) / 2 + xL;
	iy = (yB - yT - cyChar) / 2 + yT;
	iFigure = szCm[i].boomflag;
	SetTextAlign(hdc, TA_CENTER);

	if (iFigure == 1) {
		TextOut(hdc, ix, iy, TEXT("1"), 1);
	}
	else if (iFigure == 2) {
		TextOut(hdc, ix, iy, TEXT("2"), 1);
	}
	else if (iFigure == 3) {
		TextOut(hdc, ix, iy, TEXT("3"), 1);
	}
	else if (iFigure == 4) {
		TextOut(hdc, ix, iy, TEXT("4"), 1);
	}
	else if (iFigure == 5) {
		TextOut(hdc, ix, iy, TEXT("5"), 1);
	}
	else if (iFigure == 6) {
		TextOut(hdc, ix, iy, TEXT("6"), 1);
	}
}

/*
 *功能：在WM_PAINT消息中维持棋谱上的旗帜标记
 */
void fnRedrawFlag(HWND hwnd, HDC hdc) {
	int i, xL, yT, xR, yB;
	POINT apt[4];

	for (i = 0; i < szCmi[itemp].iSpecifications * szCmi[itemp].iSpecifications; i++) {
		if (szCm[i].findflag) {																						//如果玩家标记了雷
			//-------------------------------------------------------------------------------------------------初始化
			xL = rect[i].xLeft;
			yT = rect[i].yTop;
			xR = rect[i].xRight;
			yB = rect[i].yBottom;
			apt[0].x = xL + fnRoundingUp(13.0 * szCmi[itemp].iBase / 50);
			apt[0].y = yT + fnRoundingUp(8.0 * szCmi[itemp].iBase / 50);
			apt[1].x = xR - fnRoundingUp(10.0 * szCmi[itemp].iBase / 50);
			apt[1].y = yT + fnRoundingUp(18.0 * szCmi[itemp].iBase / 50);
			apt[2].x = xL + fnRoundingUp(13.0 * szCmi[itemp].iBase / 50);
			apt[2].y = yB - fnRoundingUp(22.0 * szCmi[itemp].iBase / 50);
			apt[3].x = xL + fnRoundingUp(13.0 * szCmi[itemp].iBase / 50);
			apt[3].y = yT + fnRoundingUp(8.0 * szCmi[itemp].iBase / 50);

			//-------------------------------------------------------------------------------------------------画旗帜
			hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
			hOldPen = (HPEN)SelectObject(hdc, hPen);
			hBrush = CreateBrushIndirect(&lbExpBoom);
			if (!hBrush)  MessageBox(hwnd, L"fnRedrawFlag画刷：？？？", L"老子卡了", MB_OK);
			hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
			if (hOldBrush == NULL || hOldBrush == HGDI_ERROR)  MessageBox(hwnd, L"fnRedrawFlag SelectObject：???", L"老子卡了", MB_OK);
			RoundRect(
				hdc,
				xL + fnRoundingUp(10.0 * szCmi[itemp].iBase / 50),
				yT + fnRoundingUp(5.0 * szCmi[itemp].iBase / 50),
				xL + fnRoundingUp(15.0 * szCmi[itemp].iBase / 50),
				yB - fnRoundingUp(5.0 * szCmi[itemp].iBase / 50),
				2, 2
			);
			Polygon(hdc, apt, 4);
			SelectObject(hdc, hOldPen);
			SelectObject(hdc, hOldBrush);
		}
	}
}

/*
 *功能：在WM_PAINT消息中维持棋谱上的已经暴露的雷
 */
void fnRedrawExpBomb(HDC hdc) {
	int i, xL, yT, xR, yB;
	Rect sR;

	for (i = 0; i < szCmi[itemp].iSpecifications * szCmi[itemp].iSpecifications; i++) {
		//------------------------------------------------------------------------------当该格子既存在雷，又存在重绘标记
		//-------存在重绘标记的原因是，当单击点击错误——点到了雷，fnExposureBomb()会把检索所有的雷，且置标记bombflag和drawflag为1
		if (szCm[i].boomflag == -1 && szCm[i].drawflag == 1) {

			xL = rect[i].xLeft;
			yT = rect[i].yTop;
			xR = rect[i].xRight;
			yB = rect[i].yBottom;

			hBrush = CreateBrushIndirect(&lbRemLid);
			hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
			fnDrawBomb(hdc, xL, yT, xR, yB);
			SelectObject(hdc, hOldBrush);
		}
		if (szCm[i].findflag == 1 && szCm[i].boomflag != -1) {														//检查旗帜标记时，如果不是雷	
			sR = rect[i];
			fnDrawWrongBomb(hdc, sR.xLeft, sR.yTop, sR.xRight, sR.yBottom);
		}
	}

	xL = rect[iRecentClick].xLeft;
	yT = rect[iRecentClick].yTop;
	xR = rect[iRecentClick].xRight;
	yB = rect[iRecentClick].yBottom;

	hBrush = CreateBrushIndirect(&lbExpBoom);																		//画红色的背景
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	Rectangle(hdc, xL, yT, xR, yB);
	SelectObject(hdc, hOldBrush);

	fnDrawBomb(hdc, xL, yT, xR, yB);
}

/*
 *功能：计算出雷周围8个格子的数字
 */
void fnCalculFigures() {
	int i;

	for (i = 0; i < iAmount; i++)
	{
		if (szCm[i].boomflag == -1)																					//检查雷标记
		{
			//--第一个if条件表达式判断，周围的第一个格子是否超出了范围，检查顺序依次为（逆时针）：右、右上、上、左上、左、左下、下、右下
			//--第二个if条件表达式判断，是否是最右侧或最左侧的格子，因为最右侧或最左侧的格子再向右或向左前进一个格子就超出了逻辑上的范围
			//---------------------------------------（备注：第二个if条件表达式的例外情况，正上方和正下方的格子不用判断这个条件）
			//------------------------第三个if条件表达式判断，该格子是否是雷，如果是雷数字标记（boomflag）不变，否则将数字标记叠加
			if (
				(i + szNg[itemp].iRight) < iUserChioce * iUserChioce
				&& (i + szNg[itemp].iRight) >= 0
				)
			{
				if (i % iUserChioce != (iUserChioce - 1))
				{																									//检查右侧第一个格子
					if (szCm[i + szNg[itemp].iRight].boomflag != -1)
						szCm[i + szNg[itemp].iRight].boomflag++;
				}
			}

			if (
				(i + szNg[itemp].iRightUpper) < iUserChioce * iUserChioce
				&& (i + szNg[itemp].iRightUpper) >= 0
				)
			{
				if (i % iUserChioce != (iUserChioce - 1))
				{																									//检查右上角第一个格子
					if (szCm[i + szNg[itemp].iRightUpper].boomflag != -1)
						szCm[i + szNg[itemp].iRightUpper].boomflag++;
				}
			}

			if (
				(i + szNg[itemp].iTop) < iUserChioce * iUserChioce
				&& (i + szNg[itemp].iTop) >= 0)
			{																										//检查正上方第一个格子
				if (szCm[i + szNg[itemp].iTop].boomflag != -1)
					szCm[i + szNg[itemp].iTop].boomflag++;
			}

			if (
				(i + szNg[itemp].iLeftUpper) < iUserChioce * iUserChioce
				&& (i + szNg[itemp].iLeftUpper) >= 0
				)
			{
				if (i % iUserChioce != 0)
				{																									//检查左上角第一个格子
					if (szCm[i + szNg[itemp].iLeftUpper].boomflag != -1)
						szCm[i + szNg[itemp].iLeftUpper].boomflag++;
				}
			}

			if (
				(i + szNg[itemp].iLeft) < iUserChioce * iUserChioce
				&& (i + szNg[itemp].iLeft) >= 0
				)
			{
				if (i % iUserChioce != 0)
				{																									//检查左侧第一个格子
					if (szCm[i + szNg[itemp].iLeft].boomflag != -1)
						szCm[i + szNg[itemp].iLeft].boomflag++;
				}
			}

			if (
				(i + szNg[itemp].iLeftLower) < iUserChioce * iUserChioce
				&& (i + szNg[itemp].iLeftLower) >= 0
				)
			{
				if (i % iUserChioce != 0)
				{																									//检查左下角第一个格子
					if (szCm[i + szNg[itemp].iLeftLower].boomflag != -1)
						szCm[i + szNg[itemp].iLeftLower].boomflag++;
				}
			}

			if (
				(i + szNg[itemp].iBottom) < iUserChioce * iUserChioce
				&& (i + szNg[itemp].iBottom) >= 0
				)
			{																										//检查正下方第一个格子
				if (szCm[i + szNg[itemp].iBottom].boomflag != -1)
					szCm[i + szNg[itemp].iBottom].boomflag++;
			}

			if (
				(i + szNg[itemp].iRightLower) < iUserChioce * iUserChioce
				&& (i + szNg[itemp].iRightLower) >= 0
				)
			{
				if (i % iUserChioce != (iUserChioce - 1))
				{																									//检查右下角第一个格子
					if (szCm[i + szNg[itemp].iRightLower].boomflag != -1)
						szCm[i + szNg[itemp].iRightLower].boomflag++;
				}
			}
		}
	}

}

/*
 *功能：暴露出被数字0以上包围的空白区域
 *数字边界会同时设置需要重绘的格子的drawflag标记
 *(备注：运用递归)
*/
void fnFiguresBound(int i) {
	//-------------------------------------------------------------------count记录遇到的数字，遇到1个数字就停止继续检查边界
	int count = 0;
	if (i < iUserChioce * iUserChioce && i >= 0) {																	//检查是否超出范围
		if (szCm[i].boomflag != -1 && count < 1) {																	//点击的不是雷，并且还未遇到数字
			if (szCm[i].boomflag) count++;																			//遇到第一个数字计数器加1
			szCm[i].drawflag = 1;
			szCm[i].pos = INT_MAX;																					//用无穷大标记这个格子已经被检查过
		}
	}

	if (count == 1 || i < 0 || i > iUserChioce* iUserChioce) return;

	//-----------------------------------------------------------------------------------------------以下检查的逻辑是：
	//-------------------------------------------------------范围9格内，右侧的三个（右、右上、右下）除以难度等于0说明落在界外；
	//--------------------------------------------范围9格内，左侧的三个（左、左上、左下）除以难度等于（难度 - 1）也说明落在界外；
	if (
		((i + szNg[itemp].iRight) % iUserChioce != 0)
		&& szCm[i + szNg[itemp].iRight].pos != INT_MAX
		&& ((i + szNg[itemp].iRight) >= 0 && (i + szNg[itemp].iRight) < iUserChioce * iUserChioce)
		)
		fnFiguresBound(i + szNg[itemp].iRight);

	if (
		((i + szNg[itemp].iRightUpper) % iUserChioce != 0)
		&& szCm[i + szNg[itemp].iRightUpper].pos != INT_MAX
		&& ((i + szNg[itemp].iRightUpper) >= 0 && (i + szNg[itemp].iRightUpper) < iUserChioce * iUserChioce)
		)
		fnFiguresBound(i + szNg[itemp].iRightUpper);

	if (
		szCm[i + szNg[itemp].iTop].pos != INT_MAX
		&& ((i + szNg[itemp].iTop) >= 0 && (i + szNg[itemp].iTop) < iUserChioce * iUserChioce)
		)
		fnFiguresBound(i + szNg[itemp].iTop);

	if (
		((i + szNg[itemp].iLeftUpper) % iUserChioce != (iUserChioce - 1))
		&& szCm[i + szNg[itemp].iLeftUpper].pos != INT_MAX
		&& ((i + szNg[itemp].iLeftUpper) >= 0 && (i + szNg[itemp].iLeftUpper) < iUserChioce * iUserChioce)
		)
		fnFiguresBound(i + szNg[itemp].iLeftUpper);

	if (
		((i + szNg[itemp].iLeft) % iUserChioce != (iUserChioce - 1))
		&& szCm[i + szNg[itemp].iLeft].pos != INT_MAX
		&& ((i + szNg[itemp].iLeft) >= 0 && (i + szNg[itemp].iLeft) < iUserChioce * iUserChioce)
		)
		fnFiguresBound(i + szNg[itemp].iLeft);

	if (
		((i + szNg[itemp].iLeftLower) % iUserChioce != (iUserChioce - 1))
		&& szCm[i + szNg[itemp].iLeftLower].pos != INT_MAX
		&& ((i + szNg[itemp].iLeftLower) >= 0 && (i + szNg[itemp].iLeftLower) < iUserChioce * iUserChioce)
		)
		fnFiguresBound(i + szNg[itemp].iLeftLower);

	if (
		szCm[i + szNg[itemp].iBottom].pos != INT_MAX
		&& ((i + szNg[itemp].iBottom) >= 0 && (i + szNg[itemp].iBottom) < iUserChioce * iUserChioce)
		)
		fnFiguresBound(i + szNg[itemp].iBottom);

	if (
		((i + szNg[itemp].iRightLower) % iUserChioce != 0)
		&& szCm[i + szNg[itemp].iRightLower].pos != INT_MAX
		&& ((i + szNg[itemp].iRightLower) >= 0 && (i + szNg[itemp].iRightLower) < iUserChioce * iUserChioce)
		)
		fnFiguresBound(i + szNg[itemp].iRightLower);
}

/*
 *功能：暴露出点击到的雷，并且暴露所有的雷
 */
void fnExposureBomb(HDC hdc) {

	int i;

	//-----------------------------------------------------------------------------------------------------暴露所有雷
	for (i = 0; i < szCmi[itemp].iSpecifications * szCmi[itemp].iSpecifications; i++) {
		if (szCm[i].boomflag == -1) {																				//检查是否是雷，是雷就绘制雷

			//----------------------------------------------drawflag置为1的原因是，雷也需要在重绘（重绘是白色背景）背景上绘制
			szCm[i].drawflag = 1;
		}
		if (szCm[i].findflag == 1) {
			//---------------------------检查是否是旗帜标记，是旗帜标记再检查该旗帜下的boomflag是否是雷，若不是雷说明玩家标记错误

			if (szCm[i].boomflag != -1) {																			//检查是否是雷
				szCm[i].drawflag = 1;
			}
		}
	}

}

/*
 *功能：画雷
 */
void fnDrawBomb(HDC hdc, int xL, int yT, int xR, int yB) {

	hBrush = CreateBrushIndirect(&lbBlack);																			//选择黑画刷画雷
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));																	//选择黑画笔画雷的引线
	hOldPen = (HPEN)SelectObject(hdc, hPen);

	//---------------------------------------------------------------------------------------------------------画引线
	//--(备注：iBase是每个格子的边长,这个边长乘上8/50或者15/50这些,只是乘上一个比例,这个比例是多少都没关系,目的都是在适当的地方显示雷)
	MoveToEx(
		hdc,
		(xR - xL) / 2 + xL,
		yT + fnRoundingUp(8.0 * szCmi[itemp].iBase / 50),
		NULL
	);
	LineTo(
		hdc,
		(xR - xL) / 2 + xL,
		yT + fnRoundingUp(15.0 * szCmi[itemp].iBase / 50)
	);

	//-----------------------------------------------------------------------------------------------------------画雷
	Ellipse(
		hdc,
		xL + fnRoundingUp(10.0 * szCmi[itemp].iBase / 50),
		yT + fnRoundingUp(15.0 * szCmi[itemp].iBase / 50),
		xR - fnRoundingUp(10.0 * szCmi[itemp].iBase / 50),
		yB - fnRoundingUp(5.0 * szCmi[itemp].iBase / 50)
	);
	SelectObject(hdc, hOldBrush);

	hBrush = CreateBrushIndirect(&lbRemLid);																		//选择白画刷画雷的阴影
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	//------------------------------------------------------------------------------------------------------画雷的阴影
	Ellipse(
		hdc,
		xL + fnRoundingUp(13.0 * szCmi[itemp].iBase / 50),
		yT + fnRoundingUp(20.0 * szCmi[itemp].iBase / 50),
		xR - fnRoundingUp(27.0 * szCmi[itemp].iBase / 50),
		yB - fnRoundingUp(20.0 * szCmi[itemp].iBase / 50)
	);
	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);
}

/*
 *功能：四舍五入取整数
*/
int fnRoundingUp(double dNum) {
	int iInteger;																									//整数
	double iDecimal;																								//小数

	//取小数部分
	iInteger = (int)dNum;
	iDecimal = dNum - iInteger;

	if (iDecimal >= 0.5)	iInteger++;

	return iInteger;
}

/*
 *功能：画错误标记处的雷
 *除了需要绘制雷以外，还需要绘制一个大红叉说明玩家绘制错误
 */
void fnDrawWrongBomb(HDC hdc, int xL, int yT, int xR, int yB) {

	POINT apt[] = {
		xL + fnRoundingUp(5.0 * szCmi[itemp].iBase / 50), yT + fnRoundingUp(5.0 * szCmi[itemp].iBase / 50),
		xL + fnRoundingUp(15.0 * szCmi[itemp].iBase / 50), yT + fnRoundingUp(5.0 * szCmi[itemp].iBase / 50),
		(xR - xL) / 2 + xL, yT + fnRoundingUp(20.0 * szCmi[itemp].iBase / 50),
		xR - fnRoundingUp(15.0 * szCmi[itemp].iBase / 50), yT + fnRoundingUp(5.0 * szCmi[itemp].iBase / 50),
		xR - fnRoundingUp(5.0 * szCmi[itemp].iBase / 50), yT + fnRoundingUp(5.0 * szCmi[itemp].iBase / 50),
		xR - fnRoundingUp(20.0 * szCmi[itemp].iBase / 50), (yB - yT) / 2 + yT,
		xR - fnRoundingUp(5.0 * szCmi[itemp].iBase / 50), yB - fnRoundingUp(5.0 * szCmi[itemp].iBase / 50),
		xR - fnRoundingUp(15.0 * szCmi[itemp].iBase / 50), yB - fnRoundingUp(5.0 * szCmi[itemp].iBase / 50),
		(xR - xL) / 2 + xL, yB - fnRoundingUp(20.0 * szCmi[itemp].iBase / 50),
		xL + fnRoundingUp(15.0 * szCmi[itemp].iBase / 50), yB - fnRoundingUp(5.0 * szCmi[itemp].iBase / 50),
		xL + fnRoundingUp(5.0 * szCmi[itemp].iBase / 50), yB - fnRoundingUp(5.0 * szCmi[itemp].iBase / 50),
		xL + fnRoundingUp(20.0 * szCmi[itemp].iBase / 50), (yB - yT) / 2 + yT,
		xL + fnRoundingUp(5.0 * szCmi[itemp].iBase / 50), yT + fnRoundingUp(5.0 * szCmi[itemp].iBase / 50)
	};

	//-----------------------------------------------------------------------------------------------在白色背景上绘制雷
	fnDrawBomb(hdc, xL, yT, xR, yB);

	//------------------------------------------------------------------------------------------------------绘制大红叉
	hBrush = CreateBrushIndirect(&lbExpBoom);
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	hPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
	hOldPen = (HPEN)SelectObject(hdc, hPen);
	Polygon(hdc, apt, 13);
	SelectObject(hdc, hOldBrush);
	SelectObject(hdc, hOldPen);
}

/*
 *功能：画旗子
 *会检查当前格子是否存在旗帜，如果存在右击一次取消旗帜标志，如果不存在右击一次生成旗帜标志
 */
void fnDrawFlag(HWND hwnd, HDC hdc, int normalClick) {

	if (szCm[normalClick].findflag != 1) {
		szCm[normalClick].findflag = 1;
		fFound--;
	}
	else {
		szCm[normalClick].findflag = 0;
		fFound++;
	}
}

/*
 *功能：判断胜利条件，利用fFound标记，标记为0时为胜利
 */
void fnCheckWin(HWND hwnd, HDC hdc) {
	int i, count = 0;

	if (fFound == 0) {

		for (i = 0; i < szCmi[itemp].iSpecifications * szCmi[itemp].iSpecifications; i++) {
			if (szCm[i].findflag == 1 && szCm[i].boomflag == -1) {
				count++;
				if (count == 10)	break;
			}
		}

		if (count == 10) {

			MessageBox(hwnd, TEXT("你赢了噢"), TEXT("You win"), MB_OK);
			fLLock = 1;																								//并且锁定棋谱
			fRLock = 1;

			fFound = INT_MAX;																						//真正完成了游戏置标签为无穷大
		}
		else {
			MessageBox(hwnd, TEXT("你根本就没有找全"), TEXT("我对你很失望崽"), MB_OK);
		}
	}
}