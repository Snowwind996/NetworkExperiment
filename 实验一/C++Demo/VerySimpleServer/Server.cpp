#pragma once
#include <winsock.h>
#include <stdio.h>
#include <iostream>
#include<string.h>
#include<string>
#include"Server.h"
#include<thread>
#include<regex>
#include<fstream>
#include <cassert>
#include<sstream>
using namespace std;
#define OK 0;
#define ERROR -1;
#pragma comment(lib,"ws2_32.lib")
void sendMessage(SOCKET socket, string msg) {
	int rtn = send(socket, msg.c_str(), msg.length(), 0);
	if (rtn == SOCKET_ERROR) {//发送数据错误，把产生错误的会话socekt加入sessionsClosed队列
	cout << "Send to client failed!" << endl;
//		cout << "A client leaves..." << endl;
		closesocket(socket);
	}
}
int Server::WSAsetup() {
	int nRc = WSAStartup(0x0202, &wsaData);

	if (nRc) {
		printf("Winsock  startup failed with error!\n");
		return ERROR;
	}

	if (wsaData.wVersion != 0x0202) {
		printf("Winsock version is not correct!\n");
		return ERROR;
	}

	printf("Winsock  startup Ok!\n");
	return OK;
}
int Server::configsetup() {
	cout << "_______________Command line of setup__________________" << endl;
	int read;
	string addr;
	int flag = 1;
	while (true) {
		cout << "0:testmode               1:setup         2:internetMode;" << endl;
		cin >> read;
		while (read != 0 && read != 1 && read != 2) { cout << "set up ERROR,please input correct number;"; cin >> read; }
		if (read == 0) { config.PORTNUM = 80; config.IPADDR = "127.0.0.1"; cout << "Port is:" << config.PORTNUM << endl << "Address is:" << config.IPADDR << endl; cout << "0:OK;other:again" << endl;
		cin >> flag;
		if (!flag)return OK;}
		if (read == 1) {
				cout << "Please input correct Port!\n";
				string line;
				cin >> read;
				config.PORTNUM = read;
				cout << "Please input correct ADDR!\n";
				cin >> config.IPADDR;
				cout << "Port is:" << config.PORTNUM << endl << "Address is:" << config.IPADDR << endl;
				cout << "0:OK;other:again" << endl;
				cin >> flag;
				if (!flag)return OK;}
		if (read == 2) {
			int iResult;
			DWORD dwError;
			char host_name[256];
			struct in_addr addr;
			char** pAlias; int i = 0;
			struct hostent* remoteHost;
			iResult = gethostname(host_name, sizeof(host_name));
			if (iResult != 0)
			{
				printf("gethostname failed: %d\n", iResult);
				return 1;
			}
			remoteHost = gethostbyname(host_name);
			printf("Calling gethostbyname with %s\n", host_name);
			if (remoteHost == NULL)
			{
				dwError = WSAGetLastError();
				if (dwError != 0)
				{
					if (dwError == WSAHOST_NOT_FOUND)
					{
						printf("Host not found\n");
						return 1;
					}
					else if (dwError == WSANO_DATA)
					{
						printf("No data record found\n");
						return 1;
					}
					else
					{
						printf("Function failed with error: %ld\n", dwError);
						return 1;
					}
				}
			}
			else
			{
				printf("Function returned:\n");
				printf("\tOfficial name: %s\n", remoteHost->h_name);
				for (pAlias = remoteHost->h_aliases; *pAlias != 0; pAlias++)
				{
					printf("\tAlternate name #%d: %s\n", ++i, *pAlias);
				}
				printf("\tAddress type: ");
				switch (remoteHost->h_addrtype)
				{
				case AF_INET:
					printf("AF_INET\n");
					break;
				case AF_NETBIOS:
					printf("AF_NETBIOS\n");
					break;
				default:
					printf(" %d\n", remoteHost->h_addrtype);
					break;
				}
				printf("\tAddress length: %d\n", remoteHost->h_length);
				// 如果返回的是IPv4的地址， 则输出		i = 0;		
				if (remoteHost->h_addrtype == AF_INET)
				{
					while (remoteHost->h_addr_list[i] != 0)
					{
						addr.s_addr = *(u_long*)remoteHost->h_addr_list[i++];
						printf("\tIP Address #%d: %s\n", i, inet_ntoa(addr));
					}
				}
				else if (remoteHost->h_addrtype == AF_NETBIOS)
				{
					printf("NETBIOS address was returned\n");
				}
			}
			config.IPADDR = inet_ntoa(addr);
			cout << "Please input correct PORTNUM!\n";
			cin >> config.PORTNUM;
			cout << "Port is:" << config.PORTNUM << endl << "Address is:" << config.IPADDR << endl;
			cout << "0:OK;Other:again" << endl;
			cin >> flag;
			if (!flag)return OK;
		}
	}
	return OK;
}
int Server::testring(int len) {
	cout << "接下来开始输出收到的内容" << endl;
	for (int i = 0; i < len; i++)cout << recvBuf[i];
	cout << "输出完成！" << endl;
	return OK;
};
int Server::socketsetup() {
	//create socket
	srvSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (srvSocket != INVALID_SOCKET)
		printf("Socket create Ok!\n");
	//set port and ip
	addr.sin_family = AF_INET;
	addr.sin_port = htons(config.PORTNUM);
	addr.sin_addr.S_un.S_addr = inet_addr(config.IPADDR.c_str());
	//binding
	int rtn = bind(srvSocket, (LPSOCKADDR)&addr, sizeof(addr));
	if (rtn != SOCKET_ERROR)
		printf("Socket bind Ok!\n");
	//listen
	rtn = listen(srvSocket, 5);
	if (rtn != SOCKET_ERROR)
		printf("Socket listen Ok!\n");
	clientAddr.sin_family = AF_INET;//创建一个客户端sockaddr_in，用于链接。
	addrLen = sizeof(clientAddr);
	//将srvSock设为非阻塞模式以监听客户连接请求
	if ((rtn = ioctlsocket(srvSocket, FIONBIO, &blockMode) == SOCKET_ERROR)) { //FIONBIO：允许或禁止套接口s的非阻塞模式。
		cout << "ioctlsocket() failed with error!\n";
		return OK;
	}
	cout << "ioctlsocket() for server socket ok!	Waiting for client connection and data\n";
	//清空read,write描述符，对rfds和wfds进行了初始化，必须用FD_ZERO先清空，下面才能FD_SET
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);	

	//设置等待客户连接请求
	FD_SET(srvSocket, &rfds);
};
int Server::send404() {
	char text_buf[] = "<!DOCTYPE html>\n\
<html>\n\
<head>404NotFound</head>\n\
<body>byWGJ</body>\n\
</html>";
	int rtn = send(sessionSocket, text_buf, strlen(text_buf), 0);
	return OK;
};
int Server::send400() {
	char text_buf[] = "<!DOCTYPE html>\n\
<html>\n\
<head>400Bad Request</head>\n\
<body>byWGJ</body>\n\
</html>";
	int rtn = send(sessionSocket, text_buf, strlen(text_buf), 0);
	return OK;
};
int Server::sendhead() {
	char head_buf[] = "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\n\
					 Content-Length: 1024\r\n\
					 Content-Type: text/html;charset=UTF-8\r\n\
					 Connection: close\r\n\r\n";

	int rtn = send(sessionSocket, head_buf, strlen(head_buf), 0);
	if (rtn == -1) {
		cout << "Head send failed"; return -1;
	}
	else { cout << "Head send succeed!"; }
	return OK;
};
int Server::readrcv() {
	cout << "开始正则表达式匹配！";
	string line(recvBuf);
	smatch strmatch;
	regex regulation(".*?GET /(.*?) HTTP/1.1.*?");
	int matchnum = regex_search(line, strmatch, regulation);
	if (matchnum == 0) {
		send400(); return ERROR;
	}
	cout << "match num is:" << matchnum << endl;
	cout << "请求的内容为：" << strmatch[1] << endl;
	string file = strmatch[1];
	ifstream infile;
	ostringstream buf;
	infile.open(file.data());   //将文件流对象与文件连接起来 
	if (!infile.is_open()) { send404(); return ERROR; };   //没有打开文件，输出404.
	infile >> noskipws;
	len_buf = 0;
	infile.seekg(0, ios::end);
	int len = infile.tellg();
	infile.seekg(ios::beg);
	c = new char[len]; int i = 0;
	while (!infile.eof())
	{
		infile >> c[i++];

	}
	int rtn = send(sessionSocket, c, strlen(c), 0);
	if (rtn == -1) {
		cout << "text send failed";
	}
	else { cout << "text send succeed!"; }
	infile.close();             //关闭文件输入流 
	return OK;
};
int Server::LOOP() {
	int rtn;
	while (true) {//进入服务器模式，开始死循环，重复运行。
		//清空read,write描述符
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);

		//设置等待客户连接请求
		FD_SET(srvSocket, &rfds);

		if (!first_connetion) {
			//设置等待会话SOKCET可接受数据或可发送数据
			FD_SET(sessionSocket, &rfds);
			FD_SET(sessionSocket, &wfds);
		}
		//开始等待
		int nTotal = select(0, &rfds, &wfds, NULL, NULL);
		//如果srvSock收到连接请求，接受客户连接请求
		if (FD_ISSET(srvSocket, &rfds)) {
			nTotal--;
			//产生会话SOCKET
			sessionSocket = accept(srvSocket, (LPSOCKADDR)&clientAddr, &addrLen);
			if (sessionSocket != INVALID_SOCKET)
				printf("Socket listen one client request!\n");
			//把会话SOCKET设为非阻塞模式
			if ((rtn = ioctlsocket(sessionSocket, FIONBIO, &blockMode) == SOCKET_ERROR)) { //FIONBIO：允许或禁止套接口s的非阻塞模式。
				cout << "ioctlsocket() failed with error!\n";
				return OK;
			}
			cout << "ioctlsocket() for session socket ok!	Waiting for client connection and data\n";
			//设置等待会话SOKCET可接受数据或可发送数据
			FD_SET(sessionSocket, &rfds);
			FD_SET(sessionSocket, &wfds);
			first_connetion = false;
		}
		//检查会话SOCKET是否有数据到来
		if (nTotal > 0) {
			//如果会话SOCKET有数据到来，则接受客户的数据
			if (FD_ISSET(sessionSocket, &rfds)) {
				cout << "会话socket有数据到来！" << endl;
				//receiving data from client
				std::cout << "request IP:" << (int)clientAddr.sin_addr.S_un.S_un_b.s_b1 << "." << (int)clientAddr.sin_addr.S_un.S_un_b.s_b2 << "." << (int)clientAddr.sin_addr.S_un.S_un_b.s_b3 << "." << (int)clientAddr.sin_addr.S_un.S_un_b.s_b4;
				std::cout << "\nrequest Port:" << clientAddr.sin_port << "\n";
				memset(recvBuf, '\0', 4096);
				rtn = recv(sessionSocket, recvBuf, 4096, 0);
				cout << "本次收到了" << rtn << "个字节" << endl;
				testring(rtn);
				int rt2=sendhead();
				if (rt2 == -1) {
					cout << "头部消息发送失败，关闭本次链接。"; closesocket(sessionSocket); SOCKET sessionSocket; nTotal = 0;
				}
				if (rtn != -1) {
					cout << "kai shi fa snog xiao xi";
					int rtn1=readrcv();
					if (rtn1 == -1) { cout << "本次请求有问题，发生了404或者400错误。" << endl; }
				}


			}
		}
	}
};
void main(){

	Server srv;
	srv.WSAsetup();
	int rtn=srv.configsetup();
	srv.socketsetup();
	srv.LOOP(); 



	
}