#include "client.h"
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
	//启动线程
	std::thread t1(cmdThread, _sock);
	t1.detach();
	while (g_bExit)
	{
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(_sock, &fdRead);
		timeval time = {1,0};
		int ret = select(_sock, &fdRead, NULL, NULL,&time);
		if (ret<0)
		{
			printf("failed，closed1\n");
			break;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			if (-1==ProcessorFunction(_sock))
			{
				printf("failed，closed2\n");
				break;
			}
		}
		//线程Thread
		//printf("client，working\n");
	
	}
	//接收服务器信息
	closesocket(_sock);
	WSACleanup();
	printf("client end\n");
	getchar();
	return 0;
}