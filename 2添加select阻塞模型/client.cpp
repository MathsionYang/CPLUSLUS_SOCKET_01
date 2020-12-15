#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//�����������ڿ⣬�����ͻ
#include<WinSock2.h>
#include<Windows.h>
#include<stdio.h>
//windows��
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
struct Login :public DataHeaader
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
		if (0 == strcmp(cmdBuf, "exit"))
		{
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login _log ;
			strcpy(_log.userName,"lyd");
			strcpy(_log.passWord,"1234");
			send(_sock, (const char*)&_log, sizeof(_log), 0);
			//���շ�������������
			LoginResult _logret;
			recv(_sock, (char*)&_logret, sizeof(_logret), 0);
			printf("LoginResult: %d\n", _logret.result);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			LogOut _logout;
			strcpy(_logout.userName, "lyd");
			send(_sock, (const char*)&_logout, sizeof(_logout), 0);
			LogoutResult _logOutret ;
			recv(_sock, (char*)&_logOutret, sizeof(_logOutret), 0);
			printf("LogoutResult: %d\n", _logOutret.result);
		}
		else
		{
			printf("receive unable command\n");
		}
	}
	//���շ�������Ϣ
	closesocket(_sock);
	WSACleanup();
	printf("client end\n");
	getchar();
	return 0;
}