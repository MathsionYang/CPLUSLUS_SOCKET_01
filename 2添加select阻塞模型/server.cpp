#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//避免引入早期库，引起冲突
#include<WinSock2.h>
#include<Windows.h>
#include<stdio.h>
#include<vector>
using namespace std;
//windows下
//#pragma comment(lib,"ws2_32.lib")
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR
};
struct DataHeaader
{
	short dataLength;
	short cmd;
};
struct Login:public DataHeaader
{
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];

};
struct LoginResult :public DataHeaader
{
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};
struct LogOut :public DataHeaader
{
	LogOut()
	{
		dataLength = sizeof(LogOut);
		cmd = CMD_LOGOUT;
	
	}
	char userName[32];
};
struct LogoutResult :public DataHeaader
{
	LogoutResult()
	{
		dataLength = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};
vector<SOCKET>g_client;
int ProcessorFunction(SOCKET _csock)
{
	char szRecv[1024] = {};
	//接收客户端请求
	int nlen = recv(_csock, szRecv, sizeof(DataHeaader), 0);
	DataHeaader *_header = (DataHeaader*)szRecv;
	if (nlen <= 0)
	{
		printf("client end\n");
		return -1;
	}
	switch (_header->cmd)
	{
	case CMD_LOGIN:
	{
		recv(_csock, szRecv + sizeof(DataHeaader), _header->dataLength - sizeof(DataHeaader), 0);
		Login *login = (Login*)szRecv;
		printf("Received command:  Data Length= %d  user = %s pass = %s\n", login->dataLength, login->userName, login->passWord);
		LoginResult res;
		send(_csock, (char*)&res, sizeof(LoginResult), 0);

	}
	break;
	case CMD_LOGOUT:
	{

		recv(_csock, szRecv + sizeof(DataHeaader), _header->dataLength - sizeof(DataHeaader), 0);
		LogOut *logout = (LogOut*)szRecv;
		printf("Received command: user=%s\n", logout->userName);
		LogoutResult _lgres;
		send(_csock, (char*)&_lgres, sizeof(LogoutResult), 0);

	}
	break;
	default:
		DataHeaader hd = { 0,CMD_ERROR };
		send(_csock, (const char*)&hd, sizeof(hd), 0);
		break;
	}

}
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
		int ret = select(_sock+1, &fdRead, &fdWrite,&fdEXP,NULL);
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
			printf("New Client:SOCKET = %d  IP = %s\n", _sock,inet_ntoa(clientAddr.sin_addr));
			g_client.push_back(_csock);
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