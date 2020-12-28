#include<iostream>
#include<thread>

#include<mutex>//Ëø
#include<atomic>
#include "CELLTimestamp.hpp"
using namespace std;
mutex m;
const int tconunt = 4;
int Sum = 0;
void WorkFun(int index )
{

	for (int i = 0; i < 2000; i++)
	{
		//×Ô½âËø
		lock_guard<mutex>lg(m);
		Sum++;
	}

}
int main()
{
	thread t[tconunt];
	for (int i = 0; i < tconunt; i++)
	{
		t[i] = thread(WorkFun,i);
	}
	CELLTimestamp tTime;
	for (int i = 0; i < tconunt; i++)
	{
		t[i].join();
	}
	//t.detach();
	cout<<tTime.getElapsedTimeInMilliSec()<<",SUM=" << Sum<< endl;
		cout << "hello main" << endl;
		getchar();
	return 0;
}