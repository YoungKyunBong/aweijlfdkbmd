#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE    512
SOCKET client_sock1; // 소켓
SOCKET client_sock2; // 소켓
char main_buf[BUFSIZE + 1];
int count = 0;
SOCKET main_sock[2];

static int clientOneStandXpos = 120;
static int clientOneAttackXpos = 130;
static int clientTwoStandXpos = 675;
static int clientTwoAttackXpos = 665;
int clientOneHp = 10;
int clientTwoHp = 10;

int logCount1;
int logCount2;

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 클라이언트와 데이터 통신
DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET Thread_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(Thread_sock, (SOCKADDR *)&clientaddr, &addrlen);
	
	printf("%d", count);
	while (1) {
		// 데이터 받기
		retval = recv(Thread_sock, main_buf, 4, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		if (count == 2)
		{
			if (strcmp(main_buf, "C1LG") == 0)
			{
				logCount1 = 1;
				logCount2 = 0;
			}
			else if (strcmp(main_buf, "C2LG") == 0) {
				logCount1 = 0;
				logCount2 = 1;
			}

			if (strcmp(main_buf, "C1LT") == 0 && !(clientOneStandXpos < 60))
			{
				clientOneStandXpos -= 8;
				clientOneAttackXpos -= 8;
				send(main_sock[logCount2], "C1LT", 4, 0);
				send(main_sock[logCount1], "C1LT", 4, 0);
			}
			else if (strcmp(main_buf, "C1RT") == 0 && !(clientOneStandXpos > clientTwoStandXpos))
			{
				clientOneStandXpos += 8;
				clientOneAttackXpos += 8;
				send(main_sock[logCount2], "C1RT", 4, 0);
				send(main_sock[logCount1], "C1RT", 4, 0);
			}
			else if (strcmp(main_buf, "C2LT") == 0 && !(clientOneStandXpos > clientTwoStandXpos))
			{
				clientTwoStandXpos -= 8;
				clientTwoAttackXpos -= 8;
				send(main_sock[logCount1], "C2LT", 4, 0);
				send(main_sock[logCount2], "C2LT", 4, 0);
			}
			else if (strcmp(main_buf, "C2RT") == 0 && !(clientTwoStandXpos > 735))
			{
				clientTwoStandXpos += 8;
				clientTwoAttackXpos += 8;
				send(main_sock[logCount1], "C2RT", 4, 0);
				send(main_sock[logCount2], "C2RT", 4, 0);
			}

			if (strcmp(main_buf, "C1AK") == 0)
			{
				if (clientOneAttackXpos > clientTwoStandXpos)
				{
					send(main_sock[logCount1], "C2DG", 4, 0);
					send(main_sock[logCount2], "C2DG", 4, 0);
					clientTwoHp -= 1;
					if (clientTwoHp < 0)
					{
						send(main_sock[logCount1], "YOUW", 4, 0);
					}
				}
				else {
					send(main_sock[logCount2], "NODG", 4, 0);
					Sleep(60);
					send(main_sock[logCount1], "C1ND", 4, 0);
				}
			}
			else if (strcmp(main_buf, "C2AK") == 0)
			{
				if (clientTwoAttackXpos < clientOneStandXpos)
				{
					send(main_sock[logCount1], "C1DG", 4, 0);
					send(main_sock[logCount2], "C1DG", 4, 0);
					clientOneHp -= 1;
					if (clientOneHp < 0)
					{
						send(main_sock[logCount2], "YOUW", 4, 0);
					}
				}
				else {
					send(main_sock[logCount1], "NODG", 4, 0);
					Sleep(60);
					send(main_sock[logCount2], "C2ND", 4, 0);
				}
			}
		}
		// 받은 데이터 출력
		main_buf[retval] = '\0';
		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port), main_buf);
	}
	count = 0;
	clientOneStandXpos = 120;
	clientOneAttackXpos = 130;
	clientTwoStandXpos = 675;
	clientTwoAttackXpos = 665;
	clientOneHp = 10;
	clientTwoHp = 10;
	// closesocket()
	closesocket(Thread_sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	

	return 0;
}

int main(int argc, char *argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread1;
	while(1){
		// accept()
		addrlen = sizeof(clientaddr);
		main_sock[count] = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if(main_sock[count] == INVALID_SOCKET){
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 스레드 생성
		hThread1 = CreateThread(NULL, 0, ProcessClient,
			(LPVOID)main_sock[count], 0, NULL);
		if(hThread1 == NULL) { closesocket(main_sock[count]); }
		else { CloseHandle(hThread1); }
		count++;
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}