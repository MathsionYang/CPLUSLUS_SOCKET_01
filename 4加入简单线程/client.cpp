#include "client.h"
int main()
{
	//����socket2
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//����Socket�׽���
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
	{
		printf("created SOCKET failed\n");
	}
	else
	{
		printf(" created SOCKET successed\n");
	}
	//���ӷ�����
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");////���޶�����Ip��ַ
	if (connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		printf(" failed connect \n");
	}
	else
	{
		printf("connect successed\n");
	}
	//�����߳�
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
			printf("failed��closed1\n");
			break;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			if (-1==ProcessorFunction(_sock))
			{
				printf("failed��closed2\n");
				break;
			}
		}
		//�߳�Thread
		//printf("client��working\n");
	
	}
	//���շ�������Ϣ
	closesocket(_sock);
	WSACleanup();
	printf("client end\n");
	getchar();
	return 0;
}