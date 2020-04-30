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
		MessageBox(NULL, TEXT("���������Ҫ��Windows NT����ִ�У�"), szAppName, MB_ICONERROR);
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
	static int cxClient, cyClient;																					//�ͻ����ĳ��ȺͿ��
	int xPos, yPos, firstClick;																						//xPos, yPosΪ���λ�ã�firstClickΪ����һ��λ��
	TCHAR buffer[ARR_MAX];
	size_t iTarget;

	switch (message) {
	case WM_CREATE:
	{
		hdc = GetDC(hwnd);

		//----------------------------------------------------------------------------------------------��ʼ��������Ϣ
		fnInitCMInfo();
		fnInitBrush();

		//-------------------------------------------------------------------------------------------------�����˵���
		hMenu = fnCreateMenuBar();
		SetMenu(hwnd, hMenu);

		//---------------------------------------------------------------------------------------��ȡ�ַ���ȸ߶ȵ���Ϣ
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

		//------------------------------------------------------------------------------------------------����˫������
		hMemDc = CreateCompatibleDC(hdc);																			//�����ڴ�DC
		if (!hMemDc) {
			MessageBox(hwnd, L"CreateCompatibleDC failed!", L"Failed", MB_OK);
			ReleaseDC(hwnd, hMemDc);
			DeleteDC(hMemDc);
			return 0;
		}

		hbmpMem = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);													//�����ڴ�Bmp
		if (!hbmpMem) {
			MessageBox(hwnd, L"CreatecompatibleBitmap failed!", L"Failed", MB_OK);
			DeleteObject(hbmpMem);
			return 0;
		}

		hPreBmpMem = (HBITMAP)SelectObject(hMemDc, hbmpMem);														//��ͼ����bmpѡ���豸DC

		//--------------------------------------------------------------------------------------�༭hbmpMem���������䱳��
		hBrush = CreateSolidBrush(RGB(255, 255, 255));
		FillRect(hMemDc, &rc, hBrush);

		//---------------------------------------------------------------��ʼ�����ݣ����±���������ص���Ϣ������߳��߶ȵȵ�
		fnInitLoadResource(hwnd, hMemDc, cxClient, cyClient);

		//----------------------------------------------------------------------------------------------------�ػ�����
		fnRedrawFlag(hwnd, hMemDc);
		fnRedraw(hwnd, hMemDc);

		//--------------------------------------------------------------fBoomΪ1�������������ף�fLLockΪ1��ʾ��Ϸʤ����ʧ��
		//--------------------------------------------------------------------------fFoundΪINT_MAXʱ����������ҵ�ȫ����
		//-------------------------------------------------��Ϸ��Ϊʤ����ʧ���ѽ������Ӷ����������������Ҫ��WM_PAINT��Ϣ��ά��
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
			//----------------------------------------------------------------------------����ҵ������е��ף�����ɨ������

			//---------------------------------------------------------------------------------------------��ʾ����ʱ��
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

			//------------------------------------------------------------------------------------��ʾ��ǰʣ���׵���ĿΪ0
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
			//----------------------------------------------------------------��������������Ϸ��,��ʾ��ȷ��ʱ���Լ�ʣ������Ŀ
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

		//-----------------------------------------------------------------------------------------------˫���崫��λ��
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

		//---------------------------------------------------------------------------------�ͷ�˫���������豸DC���ͼ����
		ReleaseDC(hwnd, hMemDc);
		DeleteDC(hMemDc);
		DeleteObject(hbmpMem);

		EndPaint(hwnd, &ps);
		return 0;
	}

	case WM_TIMER:
	{
		iTimerCount += 1;

		//----------------------------------------------------------------------------------------ֻ�ػ��ʱ�����ڵ�����
		rc.left = (cxClient + szCmi[itemp].iWidth) / 2 - 4 - szCmi[itemp].iBound / 15 - szCmi[itemp].iBound % 15 - szCmi[itemp].iBound / 6;
		rc.top = (cyClient - szCmi[itemp].iHeight) / 2 + 4 + szCmi[itemp].iBannerHg / 4 + szCmi[itemp].iBound / 15;
		rc.right = (cxClient + szCmi[itemp].iWidth) / 2 - 4 - szCmi[itemp].iBound / 15 - szCmi[itemp].iBound % 15 - szCmi[itemp].iBound / 12;
		rc.bottom = (cyClient - szCmi[itemp].iHeight) / 2 + 4 + szCmi[itemp].iBannerHg / 4 + szCmi[itemp].iBound / 15 + cyChar;

		InvalidateRect(hwnd, &rc, FALSE);
		return 0;
	}

	case WM_GETMINMAXINFO:
	{
		//---------------------------------------------------------------------------------------------���ƴ�����С�ߴ�
		lpmmi = (LPMINMAXINFO)lParam;
		lpmmi->ptMinTrackSize.x = 316;
		lpmmi->ptMinTrackSize.y = 439;
		return 0;
	}

	case WM_COMMAND:
		hdc = GetDC(hwnd);

		switch (LOWORD(wParam)) {
		case IDM_DEGREE_PRIMARY:																				//���Ѷ�
		{
			//---------------------------------------------------��Ҫ�ȼ���Ƿ�������Ϸ�У���Ϊ����������Ϸ�����ı��Ѷ�
			//-----------------------------------------�жϵ�������fBoom�Ƿ�Ϊ1����ΪfBoomΪ1������Ϸ�Ѿ�����ʼ����������
			if (fBoom != 1) {
				iUserChioce = PRIMARY;
				fnOnCheckMenuItem(hMenu, 0x0000);
				MoveWindow(hwnd, 80, 80, 311, 439, TRUE);
				fnDrawChessManual(hwnd, hdc, cxClient, cyClient, PRIMARY);
				fFound = szCmi[itemp].iBombAmount;																//�޸�ʣ���׵�����
			}
			else {
				MessageBox(hwnd, TEXT("������"), TEXT("��Ϸ�в�����ı��Ѷȣ�"), MB_OK);
			}
			break;
		}
		case IDM_DEGREE_INTERMEDIATE:																			//�е��Ѷ�
		{
			if (fBoom != 1) {
				iUserChioce = INTERMEDIATE;
				fnOnCheckMenuItem(hMenu, 0x0001);
				MoveWindow(hwnd, 80, 80, 410, 589, TRUE);
				fnDrawChessManual(hwnd, hdc, cxClient, cyClient, INTERMEDIATE);
				fFound = szCmi[itemp].iBombAmount;																//�޸�ʣ���׵�����
			}
			else {
				MessageBox(hwnd, TEXT("������"), TEXT("��Ϸ�в�����ı��Ѷȣ�"), MB_OK);
			}
			break;
		}
		case IDM_DEGREE_SENIOR:																					//�����Ѷ�
		{
			if (fBoom != 1) {
				iUserChioce = SENIOR;
				fnOnCheckMenuItem(hMenu, 0x0002);
				MoveWindow(hwnd, 80, 80, 650, 909, TRUE);
				fnDrawChessManual(hwnd, hdc, cxClient, cyClient, SENIOR);
				fFound = szCmi[itemp].iBombAmount;																//�޸�ʣ���׵�����
			}
			else {
				MessageBox(hwnd, TEXT("������"), TEXT("��Ϸ�в�����ı��Ѷȣ�"), MB_OK);
			}
			break;
		}
		case IDM_GAME_BEGIN:
		{
			fnRestartGame(hwnd, hdc, cxClient, cyClient);

			//----------------------------------------------------------------------------------------��ʼ����ʱ��
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

		//---------------------------------------------------------------------------------ÿ��ѡ��ͬ�Ѷȶ��ػ�������
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

		//------------------------------------------------------------------------------------------------����˫������
		hMemDc = CreateCompatibleDC(hdc);																			//�����ڴ�DC
		if (!hMemDc) {
			MessageBox(hwnd, L"CreateCompatibleDC failed!", L"Failed", MB_OK);
			DeleteDC(hMemDc);
			return 0;
		}

		hbmpMem = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);													//�����ڴ�Bmp
		if (!hbmpMem) {
			MessageBox(hwnd, L"CreatecompatibleBitmap failed!", L"Failed", MB_OK);
			DeleteObject(hbmpMem);
			return 0;
		}

		//---------------------------------------------------------------------�����ȼ������λ���Ƿ��ڲ���Ӧ�����һ�ͬ
		if (!fnCheckMousePos(cxClient, cyClient, xPos, yPos))    return 0;

		if (fBoom != 1) {																							//����fBoom������1ʱ��������δ������
			//----------------------------------------------------------------------------------------------������ʱ��
			SetTimer(hwnd, IDT_TIMER, 1000, (TIMERPROC)NULL);
			//----------------------------------------------------------------------------------------------���г�ʼ��
			firstClick = fnClientToRom(xPos, yPos);
			//----------------------------------------------------------------------------------------------���������
			fnRandomBomb(firstClick, iUserChioce);
			//---------------------------------------------------------------------------------�����漴����Χ8���ӵ�����
			fnCalculFigures();
			//---------------------------------------------------------------��fBoom��Ϊ1�����´���Ӧ���������ٽ��г�ʼ��
			fBoom = 1;
		}

		if (fLLock != 1) {																							//����Ƿ��Ѿ�ɨ���ף��ǵĻ������ټ������
			iRecentClick = fnClientToRom(xPos, yPos);

			if (szCm[iRecentClick].findflag != 1) {																	//�������ĸ����Ѿ���������ӣ��Ͳ������
				if (szCm[iRecentClick].boomflag != -1) {															//�������ĸ��Ӳ�����
					//-----------------------------------------------------��鵱ǰ�����λ�ã����������0��¶�������ֱ߽�
					fnFiguresBound(iRecentClick);
				}
				else {																								//����������
					//--------------------------------------------------------------------------------------��¶������
					fnExposureBomb(hMemDc);
					fLLock = 1;																						//������������
					fRLock = 1;
					MessageBox(hwnd, TEXT("��ɨ�����˱���"), TEXT("You lose"), MB_OK);
				}
			}
		}

		ReleaseDC(hwnd, hMemDc);
		DeleteDC(hMemDc);
		DeleteObject(hbmpMem);
		ReleaseDC(hwnd, hdc);

		//----------------------------------------------------------------------------------------------�������ػ沿��
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

		//------------------------------------------------------------------------------------------------����˫������
		hMemDc = CreateCompatibleDC(hdc);																			//�����ڴ�DC
		if (!hMemDc) {
			MessageBox(hwnd, L"CreateCompatibleDC failed!", L"Failed", MB_OK);
			DeleteDC(hMemDc);
			return 0;
		}

		hbmpMem = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);													//�����ڴ�Bmp
		if (!hbmpMem) {
			MessageBox(hwnd, L"CreatecompatibleBitmap failed!", L"Failed", MB_OK);
			DeleteObject(hbmpMem);
			return 0;
		}

		if (!fnCheckMousePos(cxClient, cyClient, xPos, yPos))    return 0;

		//-------------------------------------------------------------------fRLock��Ϊ1��û�ҽ���ȫ���ף�����û�е������
		//----------------------------------------------------------------fBoomΪ1��ʾ�Ѿ������˳�ʼ������������һ�㲻�����һ�
		if (fRLock != 1 && fBoom == 1) {

			//----------------------------------------------------------------------------------��ʼ����������������Ϣ
			iRecentClick = fnClientToRom(xPos, yPos);

			if (szCm[iRecentClick].drawflag != 1) {																	//���δ�ҿ��ĸ���

				//-------------------------------------------------��鵱ǰ��Ϸ�Ƿ񻹴����ף��Լ���鵱ǰ�����Ƿ�������ı��
				if (fFound <= 0 && szCm[iRecentClick].findflag == 0) {
					MessageBox(hwnd, TEXT("�����ȡ��һö���ı��"), TEXT("����ô����"), MB_OK);
				}
				else {//-----------------------------------------------------------------------------����������ĵ�ͼ��
					fnDrawFlag(hwnd, hMemDc, iRecentClick);
					fnCheckWin(hwnd, hMemDc);
				}
			}
		}

		ReleaseDC(hwnd, hMemDc);
		DeleteDC(hMemDc);
		DeleteObject(hbmpMem);
		ReleaseDC(hwnd, hdc);

		//---------------------------------------------------------------------------------------------�ػ�������������
		rc.left = rect[iRecentClick].xLeft;
		rc.top = rect[iRecentClick].yTop;
		rc.right = rect[iRecentClick].xRight;
		rc.bottom = rect[iRecentClick].yBottom;
		if (!InvalidateRect(hwnd, &rc, FALSE))  MessageBox(hwnd, L"RBOTTONDOWN redraw flag is failed to send WM_PAINT", L"", MB_OK);

		UpdateWindow(hwnd);

		//----------------------------------------------------------------------------------------�ػ�����"���"��������
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
 *���ܣ���ʼ��Ϸ
*/
void fnRestartGame(HWND hwnd, HDC hdc, int cxC, int cyC) {
	int i;

	//----------------------------------------------------------------------------------------------------��ʼ������־
	fFound = szCmi[itemp].iBombAmount;
	fBoom = fFigure = fLLock = fRLock = 0;

	//---------------------------------------------------------------------------------------------��ʼ��������Ϣ�ı�־
	for (i = 0; i < szCmi[itemp].iSpecifications * szCmi[itemp].iSpecifications; i++) {
		szCm[i].boomflag = szCm[i].drawflag = szCm[i].findflag = szCm[i].pos = 0;
	}

	fnDrawChessManual(hwnd, hdc, cxC, cyC, szCmi[itemp].iSpecifications);
}

/*
 *���ܣ���ʼ����ˢ��LOGBRUSH�ṹ
*/
void fnInitBrush() {

	//------------------------------------------------------------------------------------------------------��ʼ����ˢ
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
 *���ܣ��ͻ�������ת�����ڴ�λ��
*/
int fnClientToRom(int x, int y) {
	//------------------------------------------------------------------------������line����column������������ϵ�λ��pos
	//-------------------------------------in�Ǽ�¼��ǰ�����У���Ϊ��forѭ�����ж��е�������Ϊÿ�μ�9���������ҵ�ǰ���λ���ǵڶ���
	//----------------------------------------------------------����forѭ���лᵼ�µ�ǰ��Ϊ9��������Ҫһ������ı�����¼��ǰ��
	int i, line, column, pos, in = 0;
	for (i = 0; i < szCmi[itemp].iSpecifications; i++) {
		if (x >= rect[i].xLeft && x <= rect[i].xRight) {															//x�����ڵ�һ�и��ӵ���һ����������
			column = i;
			break;																									//�Ѽ�����Ŀ�����б��˳�ѭ��
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
 *���ܣ������������Ƿ�Ϊ����Ӧ����
*/
BOOL fnCheckMousePos(int cxC, int cyC, int xP, int yP) {
	//-------------------------------------------------------------------------------------xy�������λ�������ڲŸ�����Ӧ
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
 *���ܣ���ʼ������
*/
void fnInitLoadResource(HWND hwnd, HDC hdc, int cxC, int cyC) {
	fnDrawChessManual(hwnd, hdc, cxC, cyC, iUserChioce);
}

/*
 *���ܣ�����������Ϣ
 *������iwh��ȣ�iht�߶ȣ�ibd�߽糤�ȣ�ibht����߶ȣ�ibeÿ�����ӱ߳���is���׹��
 *����ע��������ͼ��ʱ���õ���Щ��Ϣ��
*/
void fnRestoreCMInfo(int iwh, int iht, int ibd, int ibht, int ibe, int is) {
	int i;

	//------------------------------------------------------------------------���ݲ���is������ж������ĸ�������Ϣ�ṹ��
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
 *���ܣ���ʼ��������Ϣ �Լ� ��ʼ����Χ9���ڸ��ӵļ����Ϣ
*/
void fnInitCMInfo() {

	//-----------------------------------------------------------------------------------------���γ�ʼ��������Ϣ�ṹ���
	//---------------------------ÿ���Ѷ������׵Ĺ��ÿ���Ѷ������׸��ӵ���������ÿ���Ѷ������������׵ķ�Χ��ÿ���Ѷ��������׵�����
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

	//--------------------------------------------------------------------------------------��ʼ����Χ9���ڸ��ӵļ����Ϣ
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
 *���ܣ���������
 *������iBound��ʾ�ɽ��ܵı߽�
*/
void fnDrawChessManual(HWND hwnd, HDC hdc, int cxC, int cyC, int iBound) {
	int ih, iw, ib, i, j, count = 0;																				//�����߶�ih�Ϳ��iw�����׳���ib
	int iRemainder = 0;																								//����ÿ�����ӵĳ���iBase��Ϊ�����������������������ᵼ�»���ĩβ�Բ�׼
	int iDlgWd;																										//��ʾ��Ǻ�ʱ����������εĿ�
	POINT sLT, sRB, sOrigin;																						//���ϽǺ����½ǵ�POINT�ṹ�Լ���ʼ������											
	size_t iTarget;

	hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	hOldPen = (HPEN)SelectObject(hdc, hPen);
	hBrush = CreateSolidBrush(RGB(211, 211, 211));
	if (!hBrush)  MessageBox(hwnd, L"fnDrawChessManual��ˢ��������", L"���ӿ���", MB_OK);
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	if (hOldBrush == NULL || hOldBrush == HGDI_ERROR)  MessageBox(hwnd, L"fnDrawChessManual SelectObject��???", L"���ӿ���", MB_OK);

	if (4 * cxC >= 3 * cyC) {
		//condition A
		ih = cyC;
		iw = 3 * ih / 4;
		ib = iw - 8;

		iBase = (ib - iBound + 1) / iBound;																			//iBase��ÿ�����ӵĳ���
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
					//----------------------------------û���ػ��ǵĸ��Ӳ���Ҫ�������ף���Ϊ���ػ��ǵĸ����Ǹ��������������
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

		iBase = (ib - iBound + 1) / iBound;																			//iBase��ÿ�����ӵĳ���
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

	//--------------------------------------------------------------------------------------����������ʾʱ��ͱ�ǵľ���
	/*SelectObject(hdc, hOldPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	hOldPen = (HPEN)SelectObject(hdc, hPen);
	SelectObject(hdc, hOldBrush);
	hBrush = CreateSolidBrush(RGB(255, 255, 255));
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);*/

	iDlgWd = ib / 5;

	SetTextAlign(hdc, TA_LEFT);
	SetBkMode(hdc, TRANSPARENT);
	StringCchLength(TEXT("���"), 100, &iTarget);
	TextOut(
		hdc,
		(cxC - iw) / 2 + 4 + ib / 15 + iDlgWd + ib / 25,
		(cyC - ih) / 2 + 4 + (ih - ib - 12) / 4 + iDlgWd / 3,
		TEXT("���"),
		iTarget
	);

	SetTextAlign(hdc, TA_RIGHT);
	StringCchLength(TEXT("���"), 100, &iTarget);
	TextOut(
		hdc,
		(cxC + iw) / 2 - 4 - ib / 15 - ib % 15 - iDlgWd - ib / 25,
		(cyC - ih) / 2 + 4 + (ih - ib - 12) / 4 + iDlgWd / 3,
		TEXT("���"),
		iTarget
	);

	if (fBoom != 1) {
		//-----------------------------------------�����׵Ŀ�ȣ��߶ȣ��߽磬���ĸ߶ȣ�ÿ�����ӵı߳��Լ��û�ѡ����Ѷȴ�������
		fnRestoreCMInfo(iw, ih, ib, ih - ib - 12, iBase, iUserChioce);
		//---------------------------------------------------------------------------------------------------ѡ���϶
		fnSelectNeighborGap(iUserChioce);
	}

}

/*
 *���ܣ������û����Ѷ�ѡ����Ӧ�ķ�Χ9�ڸ��ӵļ�϶
 *������iUserChioce
*/
void fnSelectNeighborGap(int fuc) {
	int i;

	//--------------------------------------------------------------------�ж��û�ѡ����Ѷȣ�ѡ����Ӧ�Ľṹ����Ϣ���������
	for (i = 0; i < 3; i++) {
		if (fuc == szCmi[i].iSpecifications) {																		//�ж��û�ѡ����Ѷ�
			iAmount = szCmi[i].iAmount;
			iRandomRange = szCmi[i].iRandomRange;
			iBombAmount = szCmi[i].iBombAmount;
			itemp = i;
			break;
		}
	}
}

/*
 *���ܣ������˵���
*/
HMENU fnCreateMenuBar() {
	HMENU hMenu = CreateMenu();

	HMENU hGameMenu = CreateMenu();
	AppendMenu(hGameMenu, MF_STRING, IDM_GAME_BEGIN, TEXT("&����"));
	AppendMenu(hGameMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hGameMenu, MF_STRING, IDM_GAME_EXIT, TEXT("&�˳�"));
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hGameMenu, TEXT("&�˵�"));

	HMENU hDegreeMenu = CreateMenu();
	AppendMenu(hDegreeMenu, MF_STRING | MF_CHECKED, IDM_DEGREE_PRIMARY, TEXT("&��"));
	AppendMenu(hDegreeMenu, MF_STRING, IDM_DEGREE_INTERMEDIATE, TEXT("&�е�"));
	AppendMenu(hDegreeMenu, MF_STRING, IDM_DEGREE_SENIOR, TEXT("&����"));
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hDegreeMenu, TEXT("&�Ѷ�"));

	return hMenu;
}

/*
 *���ܣ������������
 *ִ�иú����Ժ󣬵�һ�㼰��Χ8�񲻲��ף����׵ĸ��ӱ��boomflagΪ-1
 */
void fnRandomBomb(int area, int fuc) {
	int count = 0, i, j, random;																					//countΪ������,randomΪ�����
	int szCheckRepeat[625] = { 0 };																					//�������������

	//--------------------------------------------------------------------------------------------����pos�������������
	szCm[area].drawflag = 1;																						//ͬʱ�����׵������������Ϊ�ػ棬��ͬ
	szCm[area].pos = INT_MAX;																						//posΪ������������һ�����׵ĸ��ӣ���ͬ

	//-----------------------------------------------------------------------����һ�����Χ8��Ϊ������������Ҳ��Ҫ���߽�
	if (
		(area + szNg[itemp].iRight) < iUserChioce * iUserChioce
		&& (area + szNg[itemp].iRight) >= 0
		)
	{
		if ((area + szNg[itemp].iRight) % iUserChioce != 0)
		{																											//����Ҳ��һ������
			szCm[area + szNg[itemp].iRight].pos = INT_MAX;
		}
	}

	if (
		(area + szNg[itemp].iRightUpper) < iUserChioce * iUserChioce
		&& (area + szNg[itemp].iRightUpper) >= 0
		)
	{
		if ((area + szNg[itemp].iRightUpper) % iUserChioce != 0)
		{																											//������Ͻǵ�һ������
			szCm[area + szNg[itemp].iRightUpper].pos = INT_MAX;
		}
	}

	if (
		(area + szNg[itemp].iTop) < iUserChioce * iUserChioce
		&& (area + szNg[itemp].iTop) >= 0)
	{																												//������Ϸ���һ������
		szCm[area + szNg[itemp].iTop].pos = INT_MAX;
	}

	if (
		(area + szNg[itemp].iLeftUpper) < iUserChioce * iUserChioce
		&& (area + szNg[itemp].iLeftUpper) >= 0
		)
	{
		if ((area + szNg[itemp].iLeftUpper) % iUserChioce != (iUserChioce - 1))
		{																											//������Ͻǵ�һ������
			szCm[area + szNg[itemp].iLeftUpper].pos = INT_MAX;
		}
	}

	if (
		(area + szNg[itemp].iLeft) < iUserChioce * iUserChioce
		&& (area + szNg[itemp].iLeft) >= 0
		)
	{
		if ((area + szNg[itemp].iLeft) % iUserChioce != (iUserChioce - 1))
		{																											//�������һ������
			szCm[area + szNg[itemp].iLeft].pos = INT_MAX;
		}
	}

	if (
		(area + szNg[itemp].iLeftLower) < iUserChioce * iUserChioce
		&& (area + szNg[itemp].iLeftLower) >= 0
		)
	{
		if ((area + szNg[itemp].iLeftLower) % iUserChioce != (iUserChioce - 1))
		{																											//������½ǵ�һ������
			szCm[area + szNg[itemp].iLeftLower].pos = INT_MAX;
		}
	}

	if (
		(area + szNg[itemp].iBottom) < iUserChioce * iUserChioce
		&& (area + szNg[itemp].iBottom) >= 0
		)
	{																												//������·���һ������
		szCm[area + szNg[itemp].iBottom].pos = INT_MAX;
	}

	if (
		(area + szNg[itemp].iRightLower) < iUserChioce * iUserChioce
		&& (area + szNg[itemp].iRightLower) >= 0
		)
	{
		if ((area + szNg[itemp].iRightLower) % iUserChioce != 0)
		{																											//������½ǵ�һ������
			szCm[area + szNg[itemp].iRightLower].pos = INT_MAX;
		}
	}

	//-------------------------------------------------------------------------�Զ�ά����pos��ΪINT_MAX��Ԫ�ؽ�����������
	for (i = 0; i < iAmount; i++) {																					//�������������������
		if (szCm[i].pos != INT_MAX) {																				//�ṹ��Ԫ�ذ�˳������
			szCm[i].pos = count;
			count++;
		}
	}

	//----------------------------------------------------------------------------����n��������������������Ϊ�����׵ĸ���
	srand((unsigned)(time(NULL)));
	szCheckRepeat[0] = rand() % iRandomRange;
	for (i = 0; i < iBombAmount; i++) {																				//ÿ����һ��������ȼ���Ƿ��ظ�
		random = rand() % iRandomRange;																				//�ٴ�������
		for (j = 0; j < i; j++) {
			if (szCheckRepeat[j] != random) {																		//�ж��ظ�
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

	//-----------------------------------------------------------------------��������boomflag��Ϊ-1������Ԫ�ظó�Ա��ԭΪ0
	for (i = 0; i < iBombAmount; i++) {
		random = szCheckRepeat[i];																					//ȡ��һ�������
		for (j = 0; j < iAmount; j++) {																				//��ȫ��81�������жԱ�
			if (szCm[j].pos == random) {
				szCm[j].boomflag = -1;																				//���������
				break;
			}
		}
	}

	//--------------------------------------------------------------------��pos��˳���źã���figureBound()�л��õ�pos���
	for (i = 0; i < iAmount; i++) {
		szCm[i].pos = i;
	}

}

/*
 *���ܣ������ѶȲ˵����Ĺ�ѡ״̬
*/
void fnOnCheckMenuItem(HMENU hMenu, int ideg) {
	CheckMenuItem(hMenu, IDM_DEGREE_PRIMARY, MF_UNCHECKED);
	CheckMenuItem(hMenu, IDM_DEGREE_INTERMEDIATE, MF_UNCHECKED);
	CheckMenuItem(hMenu, IDM_DEGREE_SENIOR, MF_UNCHECKED);
	CheckMenuItem(hMenu, IDM_DEGREE_PRIMARY + ideg, MF_CHECKED);
}

/*
 *���ܣ��ػ����ӻ�����
 *��������drawflagΪ1�ĸ��ӽ����ػ�
 */
void fnRedraw(HWND hwnd, HDC hdc) {
	int i;

	hBrush = CreateBrushIndirect(&lbRemLid);
	if (!hBrush)  MessageBox(hwnd, L"fnRedraw��???", L"���ӿ���", MB_OK);
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	if (hOldBrush == NULL || hOldBrush == HGDI_ERROR)  MessageBox(hwnd, L"fnRedraw SelectObject��???", L"���ӿ���", MB_OK);

	for (i = 0; i < szCmi[itemp].iSpecifications * szCmi[itemp].iSpecifications; i++) {
		if (szCm[i].drawflag) {																						//ֻ�е��׵��ػ���Ϊ�ػ�ʱ�Ž����ػ�

			Rectangle(hdc, rect[i].xLeft, rect[i].yTop, rect[i].xRight, rect[i].yBottom);							//�ػ����

			fnDrawFigures(hdc, i);																					//�ػ�����
		}
	}

	SelectObject(hdc, hOldBrush);
}

/*
 *���ܣ�������
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
 *���ܣ���WM_PAINT��Ϣ��ά�������ϵ����ı��
 */
void fnRedrawFlag(HWND hwnd, HDC hdc) {
	int i, xL, yT, xR, yB;
	POINT apt[4];

	for (i = 0; i < szCmi[itemp].iSpecifications * szCmi[itemp].iSpecifications; i++) {
		if (szCm[i].findflag) {																						//�����ұ������
			//-------------------------------------------------------------------------------------------------��ʼ��
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

			//-------------------------------------------------------------------------------------------------������
			hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
			hOldPen = (HPEN)SelectObject(hdc, hPen);
			hBrush = CreateBrushIndirect(&lbExpBoom);
			if (!hBrush)  MessageBox(hwnd, L"fnRedrawFlag��ˢ��������", L"���ӿ���", MB_OK);
			hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
			if (hOldBrush == NULL || hOldBrush == HGDI_ERROR)  MessageBox(hwnd, L"fnRedrawFlag SelectObject��???", L"���ӿ���", MB_OK);
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
 *���ܣ���WM_PAINT��Ϣ��ά�������ϵ��Ѿ���¶����
 */
void fnRedrawExpBomb(HDC hdc) {
	int i, xL, yT, xR, yB;
	Rect sR;

	for (i = 0; i < szCmi[itemp].iSpecifications * szCmi[itemp].iSpecifications; i++) {
		//------------------------------------------------------------------------------���ø��Ӽȴ����ף��ִ����ػ���
		//-------�����ػ��ǵ�ԭ���ǣ�������������󡪡��㵽���ף�fnExposureBomb()��Ѽ������е��ף����ñ��bombflag��drawflagΪ1
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
		if (szCm[i].findflag == 1 && szCm[i].boomflag != -1) {														//������ı��ʱ�����������	
			sR = rect[i];
			fnDrawWrongBomb(hdc, sR.xLeft, sR.yTop, sR.xRight, sR.yBottom);
		}
	}

	xL = rect[iRecentClick].xLeft;
	yT = rect[iRecentClick].yTop;
	xR = rect[iRecentClick].xRight;
	yB = rect[iRecentClick].yBottom;

	hBrush = CreateBrushIndirect(&lbExpBoom);																		//����ɫ�ı���
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	Rectangle(hdc, xL, yT, xR, yB);
	SelectObject(hdc, hOldBrush);

	fnDrawBomb(hdc, xL, yT, xR, yB);
}

/*
 *���ܣ����������Χ8�����ӵ�����
 */
void fnCalculFigures() {
	int i;

	for (i = 0; i < iAmount; i++)
	{
		if (szCm[i].boomflag == -1)																					//����ױ��
		{
			//--��һ��if�������ʽ�жϣ���Χ�ĵ�һ�������Ƿ񳬳��˷�Χ�����˳������Ϊ����ʱ�룩���ҡ����ϡ��ϡ����ϡ������¡��¡�����
			//--�ڶ���if�������ʽ�жϣ��Ƿ������Ҳ�������ĸ��ӣ���Ϊ���Ҳ�������ĸ��������һ�����ǰ��һ�����Ӿͳ������߼��ϵķ�Χ
			//---------------------------------------����ע���ڶ���if�������ʽ��������������Ϸ������·��ĸ��Ӳ����ж����������
			//------------------------������if�������ʽ�жϣ��ø����Ƿ����ף�����������ֱ�ǣ�boomflag�����䣬�������ֱ�ǵ���
			if (
				(i + szNg[itemp].iRight) < iUserChioce * iUserChioce
				&& (i + szNg[itemp].iRight) >= 0
				)
			{
				if (i % iUserChioce != (iUserChioce - 1))
				{																									//����Ҳ��һ������
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
				{																									//������Ͻǵ�һ������
					if (szCm[i + szNg[itemp].iRightUpper].boomflag != -1)
						szCm[i + szNg[itemp].iRightUpper].boomflag++;
				}
			}

			if (
				(i + szNg[itemp].iTop) < iUserChioce * iUserChioce
				&& (i + szNg[itemp].iTop) >= 0)
			{																										//������Ϸ���һ������
				if (szCm[i + szNg[itemp].iTop].boomflag != -1)
					szCm[i + szNg[itemp].iTop].boomflag++;
			}

			if (
				(i + szNg[itemp].iLeftUpper) < iUserChioce * iUserChioce
				&& (i + szNg[itemp].iLeftUpper) >= 0
				)
			{
				if (i % iUserChioce != 0)
				{																									//������Ͻǵ�һ������
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
				{																									//�������һ������
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
				{																									//������½ǵ�һ������
					if (szCm[i + szNg[itemp].iLeftLower].boomflag != -1)
						szCm[i + szNg[itemp].iLeftLower].boomflag++;
				}
			}

			if (
				(i + szNg[itemp].iBottom) < iUserChioce * iUserChioce
				&& (i + szNg[itemp].iBottom) >= 0
				)
			{																										//������·���һ������
				if (szCm[i + szNg[itemp].iBottom].boomflag != -1)
					szCm[i + szNg[itemp].iBottom].boomflag++;
			}

			if (
				(i + szNg[itemp].iRightLower) < iUserChioce * iUserChioce
				&& (i + szNg[itemp].iRightLower) >= 0
				)
			{
				if (i % iUserChioce != (iUserChioce - 1))
				{																									//������½ǵ�һ������
					if (szCm[i + szNg[itemp].iRightLower].boomflag != -1)
						szCm[i + szNg[itemp].iRightLower].boomflag++;
				}
			}
		}
	}

}

/*
 *���ܣ���¶��������0���ϰ�Χ�Ŀհ�����
 *���ֱ߽��ͬʱ������Ҫ�ػ�ĸ��ӵ�drawflag���
 *(��ע�����õݹ�)
*/
void fnFiguresBound(int i) {
	//-------------------------------------------------------------------count��¼���������֣�����1�����־�ֹͣ�������߽�
	int count = 0;
	if (i < iUserChioce * iUserChioce && i >= 0) {																	//����Ƿ񳬳���Χ
		if (szCm[i].boomflag != -1 && count < 1) {																	//����Ĳ����ף����һ�δ��������
			if (szCm[i].boomflag) count++;																			//������һ�����ּ�������1
			szCm[i].drawflag = 1;
			szCm[i].pos = INT_MAX;																					//�����������������Ѿ�������
		}
	}

	if (count == 1 || i < 0 || i > iUserChioce* iUserChioce) return;

	//-----------------------------------------------------------------------------------------------���¼����߼��ǣ�
	//-------------------------------------------------------��Χ9���ڣ��Ҳ���������ҡ����ϡ����£������Ѷȵ���0˵�����ڽ��⣻
	//--------------------------------------------��Χ9���ڣ����������������ϡ����£������Ѷȵ��ڣ��Ѷ� - 1��Ҳ˵�����ڽ��⣻
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
 *���ܣ���¶����������ף����ұ�¶���е���
 */
void fnExposureBomb(HDC hdc) {

	int i;

	//-----------------------------------------------------------------------------------------------------��¶������
	for (i = 0; i < szCmi[itemp].iSpecifications * szCmi[itemp].iSpecifications; i++) {
		if (szCm[i].boomflag == -1) {																				//����Ƿ����ף����׾ͻ�����

			//----------------------------------------------drawflag��Ϊ1��ԭ���ǣ���Ҳ��Ҫ���ػ棨�ػ��ǰ�ɫ�����������ϻ���
			szCm[i].drawflag = 1;
		}
		if (szCm[i].findflag == 1) {
			//---------------------------����Ƿ������ı�ǣ������ı���ټ��������µ�boomflag�Ƿ����ף���������˵����ұ�Ǵ���

			if (szCm[i].boomflag != -1) {																			//����Ƿ�����
				szCm[i].drawflag = 1;
			}
		}
	}

}

/*
 *���ܣ�����
 */
void fnDrawBomb(HDC hdc, int xL, int yT, int xR, int yB) {

	hBrush = CreateBrushIndirect(&lbBlack);																			//ѡ��ڻ�ˢ����
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));																	//ѡ��ڻ��ʻ��׵�����
	hOldPen = (HPEN)SelectObject(hdc, hPen);

	//---------------------------------------------------------------------------------------------------------������
	//--(��ע��iBase��ÿ�����ӵı߳�,����߳�����8/50����15/50��Щ,ֻ�ǳ���һ������,��������Ƕ��ٶ�û��ϵ,Ŀ�Ķ������ʵ��ĵط���ʾ��)
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

	//-----------------------------------------------------------------------------------------------------------����
	Ellipse(
		hdc,
		xL + fnRoundingUp(10.0 * szCmi[itemp].iBase / 50),
		yT + fnRoundingUp(15.0 * szCmi[itemp].iBase / 50),
		xR - fnRoundingUp(10.0 * szCmi[itemp].iBase / 50),
		yB - fnRoundingUp(5.0 * szCmi[itemp].iBase / 50)
	);
	SelectObject(hdc, hOldBrush);

	hBrush = CreateBrushIndirect(&lbRemLid);																		//ѡ��׻�ˢ���׵���Ӱ
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	//------------------------------------------------------------------------------------------------------���׵���Ӱ
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
 *���ܣ���������ȡ����
*/
int fnRoundingUp(double dNum) {
	int iInteger;																									//����
	double iDecimal;																								//С��

	//ȡС������
	iInteger = (int)dNum;
	iDecimal = dNum - iInteger;

	if (iDecimal >= 0.5)	iInteger++;

	return iInteger;
}

/*
 *���ܣ��������Ǵ�����
 *������Ҫ���������⣬����Ҫ����һ������˵����һ��ƴ���
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

	//-----------------------------------------------------------------------------------------------�ڰ�ɫ�����ϻ�����
	fnDrawBomb(hdc, xL, yT, xR, yB);

	//------------------------------------------------------------------------------------------------------���ƴ���
	hBrush = CreateBrushIndirect(&lbExpBoom);
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	hPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
	hOldPen = (HPEN)SelectObject(hdc, hPen);
	Polygon(hdc, apt, 13);
	SelectObject(hdc, hOldBrush);
	SelectObject(hdc, hOldPen);
}

/*
 *���ܣ�������
 *���鵱ǰ�����Ƿ�������ģ���������һ�һ��ȡ�����ı�־������������һ�һ���������ı�־
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
 *���ܣ��ж�ʤ������������fFound��ǣ����Ϊ0ʱΪʤ��
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

			MessageBox(hwnd, TEXT("��Ӯ����"), TEXT("You win"), MB_OK);
			fLLock = 1;																								//������������
			fRLock = 1;

			fFound = INT_MAX;																						//�����������Ϸ�ñ�ǩΪ�����
		}
		else {
			MessageBox(hwnd, TEXT("�������û����ȫ"), TEXT("�Ҷ����ʧ����"), MB_OK);
		}
	}
}