#include "EasyTcpClient.hpp"
#include<thread>

bool g_bRun = true;
void cmdThread()
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			printf("退出cmdThread线程\n");
			break;
		}
		else {
			printf("不支持的命令。\n");
		}
	}
}
//客户端数量
const int cCount = 1000;
//线程数量
const int tCount = 4;
//客户端数组
EasyTcpClient* client[cCount];
void sendThread(int index_id)
{
	int everythreadNums = cCount / tCount;
	int begin = (index_id - 1)*everythreadNums;
	int end = index_id * everythreadNums;
	for (int n = begin; n < end; n++)
	{
		if (!g_bRun)
		{
			return;
		}
		client[n] = new EasyTcpClient();
	}
	for (int n = begin; n < end; n++)
	{
		if (!g_bRun)
		{
			return;
		}
		client[n]->Connect("127.0.0.1", 4567);
		printf("Connect = %d\n", n);
	}


	Login login;
	strcpy(login.userName, "lyd");
	strcpy(login.PassWord, "lydmm");
	while (g_bRun)
	{
		for (int n = begin; n < end; n++)
		{
			client[n]->SendData(&login);
			client[n]->OnRun();
		}

	}

	for (int n = begin; n < end; n++)
	{
		client[n]->Close();
	}

}

int main()
{


	//启动Ui线程
	std::thread t1(cmdThread);
	t1.detach();
	//启动发送线程
	for (int n = 0; n < tCount; n++)
	{
		std::thread t1(sendThread,n+1);
		t1.detach();
	}
	while (g_bRun)
	{
		Sleep(100);
	}
	printf("已退出。\n");
	return 0;
}