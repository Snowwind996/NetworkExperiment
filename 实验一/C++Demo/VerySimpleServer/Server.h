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
	fd_set rfds;				//用于检查socket是否有数据到来的的文件描述符，用于socket非阻塞模式下等待网络事件通知（有数据到来）
	fd_set wfds;				//用于检查socket是否可以发送的文件描述符，用于socket非阻塞模式下等待网络事件通知（可以发送数据）
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