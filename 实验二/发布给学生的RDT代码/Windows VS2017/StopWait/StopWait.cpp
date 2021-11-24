// StopWait.cpp : �������̨Ӧ�ó������ڵ㡣
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
	printf("��ӭʹ�������ģ��ϵͳ\n");
	printf("��������Ҫģ���Э��\n1:GBN,2:SR,3:TCP,4:RDT\n");
	int choice;
	cin >> choice;
	while (1) {
		printf("��ʼ����ѭ����");
		if (choice == 1) {
			printf("�������ǣ�����GBN����ģʽ��\n�����뻬�����ڵĴ�С��");
			int number;
			scanf("%d",&number);
			GBNSender* ps = new GBNSender(10,number);
			GBNReceiver* pr = new GBNReceiver(10);
			//pns->setRunMode(0);  //VERBOSģʽ
			pns->setRunMode(1);  //����ģʽ
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
			delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
			delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete
			printf("����.\n");
			return 0;
		}
		if (choice == 2) {
			printf("�������ǣ�����GBN����ģʽ��\n�����뻬�����ڵĴ�С��");
			int number;
			scanf("%d", &number);
			SRSender* ps = new SRSender(10,number);
			SRReceiver* pr = new SRReceiver(10,number);
			//pns->setRunMode(0);  //VERBOSģʽ
			pns->setRunMode(1);  //����ģʽ
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
			delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
			delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete
			return 0;
		}
		if (choice == 3) {
			printf("�������ǣ�����TCP����ģʽ��\n�����뻬�����ڵĴ�С��");
			int number;
			scanf("%d", &number);
			TCPSender* ps = new TCPSender(10, number);
			TCPReceiver* pr = new TCPReceiver(10);
			//pns->setRunMode(0);  //VERBOSģʽ
			pns->setRunMode(1);  //����ģʽ
			pns->init();
			pns->setRtdSender(ps);
			pns->setRtdReceiver(pr);
			pns->setInputFile(".\\input1.txt");
			pns->setOutputFile(".\\output1.txt");
			pns->start();
			printf("hello,world2!\n");
			delete ps;
			delete pr;
			delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
			delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete
			return 0;
		}
		if (choice == 4) {
			RdtSender* ps = new StopWaitRdtSender();
			RdtReceiver* pr = new StopWaitRdtReceiver();
			//	pns->setRunMode(0);  //VERBOSģʽ
	
		}
		else {
			printf("�����������������룡");
		}

	}
	printf("�������󣡽�������");
	return 0;
}

