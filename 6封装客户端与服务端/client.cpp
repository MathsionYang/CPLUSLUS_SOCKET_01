#include "EasyTcpClient.hpp"
#include<thread>

void cmdThread(EasyTcpClient* client)
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			client->Close();
			printf("�˳�cmdThread�߳�\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "lyd");
			strcpy(login.PassWord, "lydmm");
			client->SendData(&login);

		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			Logout logout;
			strcpy(logout.userName, "lyd");
			client->SendData(&logout);
		}
		else {
			printf("��֧�ֵ����\n");
		}
	}
}

int main()
{

	EasyTcpClient client;
	client.Connect("127.0.0.1", 4567);

	//����UI�߳�
	std::thread t1(cmdThread, &client);
	t1.detach();


	while (client.isRun())
	{
		client.OnRun();
		//printf("����ʱ�䴦������ҵ��..\n");
		//Sleep(1000);
	}
	client.Close();

	printf("���˳���\n");
	getchar();
	return 0;
}