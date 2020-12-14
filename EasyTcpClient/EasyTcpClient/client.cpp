#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//避免引入早期库，引起冲突
#include<WinSock2.h>
#include<Windows.h>
#include<stdio.h>
//windows下
//#pragma comment(lib,"ws2_32.lib")

int main()
{
	//启动socket2
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//建立Socket套接字
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
	{
		printf("created SOCKET failed\n");
	}
	else
	{
		printf(" created SOCKET successed\n");
	}
	//链接服务器
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");////不限定访问Ip地址
	if (connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		printf(" failed connect \n");
	}
	else
	{
		printf("connect successed\n");
	}
	
	while (true)
	{
		char cmdBuf[128] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf,"exit"))
		{
			break;
		}
		else
		{
			//向服务器发送请求
			send(_sock, cmdBuf, strlen(cmdBuf) + 1, 0);
		}
		char  recvBuf[256] = {};
		int nlen = recv(_sock, recvBuf, 256, 0);
		if (nlen > 0)
		{
			printf("receive data successed\n");
			printf("received data: %s \n", recvBuf);
		}
		else
		{
			printf("receive data failed\n");
		}
	}
	//接收服务器信息
	

	closesocket(_sock);
	WSACleanup();
	printf("client end\n");
	getchar();
	return 0;
}
