#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include "resource.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 512
#define VK_Z 0x5A 

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawBitmap(HDC hdc, int x, int y, HBITMAP hBit);
void err_quit(char *msg);
void err_display(char *msg);
DWORD WINAPI ClientMain(LPVOID arg);

SOCKET sock; // 소켓
char buf[BUFSIZE + 1]; // 데이터 송수신 버퍼
HINSTANCE g_hInst;
int retval;
LPCTSTR lpszClass = TEXT("Client2");

static int clientTwoXpos = 670;
static int clientTwoYpos = 165;
static int clientTwoAttackXpos = 630;
static int clientTwoAttackYpos = 146;

static int clientOneXpos = 70;
static int clientOneYpos = 165;
static int clientOneAttackXpos = 70;
static int clientOneAttackYpos = 146;

int clientOneMotionCount = 0;
int clientTwoMotionCount = 0;

int clientOneHp = 10;
int clientTwoHp = 10;

BOOL WinTheGame = FALSE;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	// 소켓 통신 스레드 생성
	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		100, 100, 768 + 16, 224 + 39, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	ZeroMemory(&Message, sizeof(Message));

	while (true)
	{
		if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
		{
			if (Message.message == WM_QUIT)
			{
				break;
			}
			else
			{
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			}
		}
		else
		{
			//InvalidateRect(hWnd, NULL, TRUE);
		}
	}


	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return (int)Message.wParam;
}

void DrawBitmap(HDC hdc, int x, int y, HBITMAP hBit)
{
	HDC MemDC, BackDC;
	HBITMAP OldBitmap, BackBitmap;
	int bx, by;
	BITMAP bit;

	GetObject(hBit, sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;

	MemDC = CreateCompatibleDC(hdc);
	BackBitmap = CreateCompatibleBitmap(hdc, 768, 224);
	OldBitmap = (HBITMAP)SelectObject(MemDC, BackBitmap);

	BackDC = CreateCompatibleDC(hdc);
	SelectObject(BackDC, hBit);

	BitBlt(MemDC, x, y, bx, by, BackDC, 0, 0, SRCCOPY);
	DeleteObject(BackDC);

	BitBlt(hdc, x, y, bx, by, MemDC, 0, 0, SRCCOPY);
	SelectObject(MemDC, OldBitmap);

	DeleteObject(BackBitmap);
	DeleteDC(MemDC);
}
void DrawGreenStand(HDC hdc, int x, int y, HBITMAP hBit)
{
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx, by;
	BITMAP bit;

	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit, sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;

	GdiTransparentBlt(hdc, x, y, 55, 55, MemDC, 14, 13, 35, 36, RGB(255, 0, 255));

	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}
void DrawGreenAttack(HDC hdc, int x, int y, HBITMAP hBit)
{
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx, by;
	BITMAP bit;

	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit, sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;

	GdiTransparentBlt(hdc, x, y, 75, 75, MemDC, 0, 0, 50, 50, RGB(255, 0, 255));

	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}
void DrawShadowStand(HDC hdc, int x, int y, HBITMAP hBit)
{
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx, by;
	BITMAP bit;

	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit, sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;

	GdiTransparentBlt(hdc, x, y, 55, 55, MemDC, 14, 13, 35, 36, RGB(255, 0, 255));
	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}
void DrawShadowAttack(HDC hdc, int x, int y, HBITMAP hBit)
{
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx, by;
	BITMAP bit;

	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit, sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;

	GdiTransparentBlt(hdc, x, y, 75, 75, MemDC, 0, 0, 50, 50, RGB(255, 0, 255));
	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	static HBITMAP  ShadowAttackBitmap, GreenAttackBitmap, ShadowStandingBitmap, GreenStandingBitmap, mainBitmap;
	PAINTSTRUCT ps;

	switch (iMessage)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
			Sleep(20);
		case VK_LEFT:
			send(sock, "C2LT", 4, 0);
			break;
		case VK_RIGHT:
			send(sock, "C2RT", 4, 0);
			break;
		case VK_CONTROL:
			if (clientTwoMotionCount == 0)
			{
				send(sock, "C2AK", 4, 0);
			}
			break;
		case VK_ESCAPE:
			exit(1);
			break;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_CONTROL:
			clientTwoMotionCount = 0;
		}
		return 0;
	case WM_CREATE:
		mainBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
		GreenStandingBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));
		ShadowStandingBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP6));
		GreenAttackBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));
		ShadowAttackBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP7));
		SetTimer(hWnd, 1, 25, NULL);
		return 0;
	case WM_TIMER:
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawBitmap(hdc, 0, 0, mainBitmap);
		if (WinTheGame == TRUE)
		{
			TextOut(hdc, 350, 50, TEXT("2P WIN"), 6);
		}

		if (clientOneMotionCount == 0)
		{
			DrawGreenStand(hdc, clientOneXpos, clientOneYpos, GreenStandingBitmap);
		}
		else if (clientOneMotionCount == 1) {
			DrawShadowStand(hdc, clientTwoXpos, clientTwoYpos, ShadowStandingBitmap);
			DrawGreenAttack(hdc, clientOneAttackXpos, clientOneAttackYpos, GreenAttackBitmap);
			Sleep(200);
			clientOneMotionCount = 0;
		}

		if (clientTwoMotionCount == 0)
		{
			DrawShadowStand(hdc, clientTwoXpos, clientTwoYpos, ShadowStandingBitmap);
		}
		else if (clientTwoMotionCount == 1)
		{
			DrawShadowAttack(hdc, clientTwoAttackXpos, clientTwoAttackYpos, ShadowAttackBitmap);
			Sleep(200);
			clientTwoMotionCount = 2;
		} else if (clientTwoMotionCount == 1) {
			DrawGreenAttack(hdc, clientOneAttackXpos, clientOneAttackYpos, GreenAttackBitmap);
			Sleep(200);
			clientTwoMotionCount = 2;
		}
		else if (clientTwoMotionCount == 2)
		{
			DrawShadowStand(hdc, clientTwoXpos, clientTwoYpos, ShadowStandingBitmap);
		}

		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		KillTimer(hWnd, 1);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

DWORD WINAPI ClientMain(LPVOID arg)
{

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	send(sock, "C2LG", 4, 0);
	// 서버와 데이터 통신
	while (1) {
		// 데이터 받기
		Sleep(20);
		retval = recv(sock, buf, 4, 0);
		if (strcmp(buf, "C2LT") == 0)
		{
			clientTwoXpos -= 8;
			clientTwoAttackXpos -= 8;
		}
		else if (strcmp(buf, "C2RT") == 0)
		{
			clientTwoXpos += 8;
			clientTwoAttackXpos += 8;
		}

		if (strcmp(buf, "C1LT") == 0)
		{
			clientOneXpos -= 8;
			clientOneAttackXpos -= 8;
		}
		else if (strcmp(buf, "C1RT") == 0)
		{
			clientOneXpos += 8;
			clientOneAttackXpos += 8;
		}
		
		if (strcmp(buf, "C2DG") == 0)
		{
			clientOneMotionCount = 1;
			clientTwoHp -= 1;
			printf("2P 남은 체력: %d \n", clientTwoHp);
		}
		else if (strcmp(buf, "C1DG") == 0)
		{
			clientOneHp -= 1;
			clientTwoMotionCount = 1;
			printf("1P 남은 체력: %d \n", clientOneHp);
		}
		else if (strcmp(buf, "NODG") == 0)
		{
			clientOneMotionCount = 1;
		}
		else if (strcmp(buf, "C2ND") == 0)
		{
			clientTwoMotionCount = 1;
		}
		else if (strcmp(buf, "YOUW") == 0)
		{
			WinTheGame = TRUE;
		}
		printf("%s \n", buf);
	}

	return 0;
}