// StopWait.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "StopWaitRdtSender.h"
#include "StopWaitRdtReceiver.h"
#include "GBNReceiver.h"
#include "GBNSender.h"
#include "SRSender.h"
#include "SRReceiver.h"
#include "TCPSender.h"
#include "TCPReceiver.h"
int main(int argc, char* argv[])
{
	printf("欢迎使用运输层模拟系统\n");
	printf("请输入想要模拟的协议\n1:GBN,2:SR,3:TCP,4:RDT\n");
	int choice;
	cin >> choice;
	while (1) {
		printf("开始进入循环！");
		if (choice == 1) {
			printf("德玛西亚！启动GBN发送模式！\n请输入滑动窗口的大小！");
			int number;
			scanf("%d",&number);
			GBNSender* ps = new GBNSender(10,number);
			GBNReceiver* pr = new GBNReceiver(10);
			//pns->setRunMode(0);  //VERBOS模式
			pns->setRunMode(1);  //安静模式
			pns->init();
			pns->setRtdSender(ps);
			pns->setRtdReceiver(pr);
			pns->setInputFile(".\\input1.txt");
			pns->setOutputFile(".\\output1.txt");
			printf("hello,world1!\n");
			pns->start();
			printf("hello,world2!\n");
			delete ps;
			delete pr;
			delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
			delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
			printf("结束.\n");
			return 0;
		}
		if (choice == 2) {
			printf("德玛西亚！启动GBN发送模式！\n请输入滑动窗口的大小！");
			int number;
			scanf("%d", &number);
			SRSender* ps = new SRSender(10,number);
			SRReceiver* pr = new SRReceiver(10,number);
			//pns->setRunMode(0);  //VERBOS模式
			pns->setRunMode(1);  //安静模式
			pns->init();
			pns->setRtdSender(ps);
			pns->setRtdReceiver(pr);
			pns->setInputFile(".\\input1.txt");
			pns->setOutputFile(".\\output1.txt");
			printf("hello,world1!\n");
			pns->start();
			printf("hello,world2!\n");
			delete ps;
			delete pr;
			delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
			delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
			return 0;
		}
		if (choice == 3) {
			printf("德玛西亚！启动TCP发送模式！\n请输入滑动窗口的大小！");
			int number;
			scanf("%d", &number);
			TCPSender* ps = new TCPSender(10, number);
			TCPReceiver* pr = new TCPReceiver(10);
			//pns->setRunMode(0);  //VERBOS模式
			pns->setRunMode(1);  //安静模式
			pns->init();
			pns->setRtdSender(ps);
			pns->setRtdReceiver(pr);
			pns->setInputFile(".\\input1.txt");
			pns->setOutputFile(".\\output1.txt");
			pns->start();
			printf("hello,world2!\n");
			delete ps;
			delete pr;
			delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
			delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
			return 0;
		}
		if (choice == 4) {
			RdtSender* ps = new StopWaitRdtSender();
			RdtReceiver* pr = new StopWaitRdtReceiver();
			//	pns->setRunMode(0);  //VERBOS模式
	
		}
		else {
			printf("输入有误！请重新输入！");
		}

	}
	printf("输入有误！结束程序！");
	return 0;
}

