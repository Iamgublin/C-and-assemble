// ConsoleApplication1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<WinSock2.h>
#include<iostream>
#include<WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA data;
	SOCKET sock;
	SOCKET sockrev;
	sockaddr_in sockadd;
	char* buf = (char*)malloc(100);
	char* buf1 = (char*)malloc(100);
	int port = 81;
	sockadd.sin_family = AF_INET;
	sockadd.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	sockadd.sin_port = htons(port);
	WSAStartup(MAKEWORD(2, 2), &data);
	printf("%d\n", WSAGetLastError());
	printf("%s\n", data.szDescription);
	printf("%s\n", data.szSystemStatus);
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	printf("%d\n", WSAGetLastError());
	sockrev = connect(sock, (LPSOCKADDR)&sockadd, sizeof(sockadd));
	printf("%d\n", WSAGetLastError());
	while (true)
	{
		scanf("%s", buf);
		if (strncmp(buf, "quit", sizeof("quit")) == 0)
		{
			send(sock, buf, 100, 0);
			break;
		}
		send(sock, buf, 100, 0);                               //客户端为自己的端口
		recv(sock, buf1, 100, 0);
		printf("receive message:%s\n", buf1);
	}
	Sleep(10000);
	return 0;
}

