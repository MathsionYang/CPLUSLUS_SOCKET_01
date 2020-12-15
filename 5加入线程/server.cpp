#include "server.h"
int main()
{
	//启动socket2
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//建立Socket套接字
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
		printf("Bind Port Sucessed\n");
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
	char _recvbuf[128] = {};
	while (true)
	{
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdEXP;
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdEXP);

		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdEXP);

		for (int i = g_client.size()-1; i >=0; i--)
		{
			FD_SET(g_client[i], &fdRead);
		}
		timeval time = {1,0};
		int ret = select(_sock+1, &fdRead, &fdWrite,&fdEXP, &time);
		if (ret<0)
		{
			printf("failed，closed\n");
			break;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			//accept等待客户端链接
			sockaddr_in clientAddr = {};
			int nAddrLen = sizeof(clientAddr);
			SOCKET _csock = INVALID_SOCKET;

			_csock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
			if (_csock == INVALID_SOCKET) {
				printf("failed，Invalid client received\n");
			}
			else
			{
				for (int n = g_client.size() - 1; n >= 0; n--)
				{
					NewUser userJoin;
					send(g_client[n], (const char*)&userJoin, sizeof(NewUser), 0);
				}
				printf("New Client:SOCKET = %d  IP = %s\n", _csock, inet_ntoa(clientAddr.sin_addr));
				g_client.push_back(_csock);
			}
 		}
		for (int k = 0; k < fdRead.fd_count; k++)
		{
			if (-1 == ProcessorFunction(g_client[k]))
			{
				auto iter = find(g_client.begin(), g_client.end(), fdRead.fd_array[k]);
				if (iter != g_client.end())
				{
					g_client.erase(iter);
				}
			}
		}
		printf("other work doing\n");

	}
	for (int k = 0; k < g_client.size(); k++)
	{
		closesocket(g_client[k]);
	}
	closesocket(_sock);
	WSACleanup();
	printf("server end\n");
	getchar();
	return 0;
}