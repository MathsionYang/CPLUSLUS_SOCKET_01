#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//�����������ڿ⣬�����ͻ
#include<WinSock2.h>
#include<Windows.h>
#include<stdio.h>
//windows��
//#pragma comment(lib,"ws2_32.lib")

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
			//���������������
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
	//���շ�������Ϣ
	

	closesocket(_sock);
	WSACleanup();
	printf("client end\n");
	getchar();
	return 0;
}
