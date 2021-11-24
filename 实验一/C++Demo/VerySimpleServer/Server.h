#pragma once
#include <winsock.h>
#include <stdio.h>
#include <iostream>
#include<string.h>
#include<string>
#include"Server.h"
using namespace std;
#define OK 0;
#pragma comment(lib,"ws2_32.lib")
struct Config {
	string IPADDR;
	int PORTNUM;
};

class Server {
public:
	int WSAsetup();
	int socketsetup();
	int configsetup();
	int LOOP();
private:
	WSADATA wsaData;
	fd_set rfds;				//���ڼ��socket�Ƿ������ݵ����ĵ��ļ�������������socket������ģʽ�µȴ������¼�֪ͨ�������ݵ�����
	fd_set wfds;				//���ڼ��socket�Ƿ���Է��͵��ļ�������������socket������ģʽ�µȴ������¼�֪ͨ�����Է������ݣ�
	bool first_connetion = true;
	SOCKET srvSocket;
	sockaddr_in addr, clientAddr;
	SOCKET sessionSocket;
	int addrLen;
	Config config;
	u_long blockMode = 1;
	char recvBuf[4096];
	int testring(int len);
	int send404();
	int sendhead();
	int send400();
	int readrcv();
	char *c;
	int len_buf = 0;
	int getclientinfo();
};