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
	WORD ver = MAKEWORD(2,2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//建立Socket套接字
	SOCKET _sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	//绑定端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");////不限定访问Ip地址
	
	if (bind(_sock, (sockaddr*)&_sin, sizeof(_sin)) == SOCKET_ERROR)
	{
		printf("Bind Port failed\n");
	}
	else
	{
		printf("Bind Port failed\n");
	}
	//listen监听网络端口
	if (listen(_sock, 5) == SOCKET_ERROR)
	{
		printf("Listen Port Sucessed\n");
	}
	else
	{
		printf("Listen Port Sucessed\n");
	}
	//accept等待客户端链接
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(clientAddr);
	SOCKET _csock = INVALID_SOCKET;
	
	_csock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (_csock == INVALID_SOCKET) {
		printf("failed，Invalid client received\n");
	}
	printf("New Client:IP = %s\n", inet_ntoa(clientAddr.sin_addr));
	
	char _recvbuf[128] = {};
	while (true)
	{
		//接收客户端请求
		int nlen = recv(_csock, _recvbuf,128,0);
		if (nlen<=0)
		{
			printf("client end\n");
			break;
		}
		printf("Received command: %s\n", _recvbuf);
		//处理请求
		if (0 == strcmp(_recvbuf,"getName"))
		{
			char msgbuf[] = "yang";
			send(_csock, msgbuf, strlen(msgbuf) + 1, 0);
		}
		else if (0 == strcmp(_recvbuf, "getAge"))
		{
			char msgbuf[] = "25";
			send(_csock, msgbuf, strlen(msgbuf) + 1, 0);
		}
		else
		{
			char msgbuf[] = "what are u doing?";
			send(_csock, msgbuf, strlen(msgbuf) + 1, 0);
		}
		
	}
	closesocket(_sock);
	WSACleanup();
	printf("server end\n");
	getchar();
	return 0;
}
