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
	if (rtn == SOCKET_ERROR) {//�������ݴ��󣬰Ѳ�������ĻỰsocekt����sessionsClosed����
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
				// ������ص���IPv4�ĵ�ַ�� �����		i = 0;		
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
	cout << "��������ʼ����յ�������" << endl;
	for (int i = 0; i < len; i++)cout << recvBuf[i];
	cout << "�����ɣ�" << endl;
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
	clientAddr.sin_family = AF_INET;//����һ���ͻ���sockaddr_in���������ӡ�
	addrLen = sizeof(clientAddr);
	//��srvSock��Ϊ������ģʽ�Լ����ͻ���������
	if ((rtn = ioctlsocket(srvSocket, FIONBIO, &blockMode) == SOCKET_ERROR)) { //FIONBIO��������ֹ�׽ӿ�s�ķ�����ģʽ��
		cout << "ioctlsocket() failed with error!\n";
		return OK;
	}
	cout << "ioctlsocket() for server socket ok!	Waiting for client connection and data\n";
	//���read,write����������rfds��wfds�����˳�ʼ����������FD_ZERO����գ��������FD_SET
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);	

	//���õȴ��ͻ���������
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
	cout << "��ʼ������ʽƥ�䣡";
	string line(recvBuf);
	smatch strmatch;
	regex regulation(".*?GET /(.*?) HTTP/1.1.*?");
	int matchnum = regex_search(line, strmatch, regulation);
	if (matchnum == 0) {
		send400(); return ERROR;
	}
	cout << "match num is:" << matchnum << endl;
	cout << "���������Ϊ��" << strmatch[1] << endl;
	string file = strmatch[1];
	ifstream infile;
	ostringstream buf;
	infile.open(file.data());   //���ļ����������ļ��������� 
	if (!infile.is_open()) { send404(); return ERROR; };   //û�д��ļ������404.
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
	infile.close();             //�ر��ļ������� 
	return OK;
};
int Server::LOOP() {
	int rtn;
	while (true) {//���������ģʽ����ʼ��ѭ�����ظ����С�
		//���read,write������
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);

		//���õȴ��ͻ���������
		FD_SET(srvSocket, &rfds);

		if (!first_connetion) {
			//���õȴ��ỰSOKCET�ɽ������ݻ�ɷ�������
			FD_SET(sessionSocket, &rfds);
			FD_SET(sessionSocket, &wfds);
		}
		//��ʼ�ȴ�
		int nTotal = select(0, &rfds, &wfds, NULL, NULL);
		//���srvSock�յ��������󣬽��ܿͻ���������
		if (FD_ISSET(srvSocket, &rfds)) {
			nTotal--;
			//�����ỰSOCKET
			sessionSocket = accept(srvSocket, (LPSOCKADDR)&clientAddr, &addrLen);
			if (sessionSocket != INVALID_SOCKET)
				printf("Socket listen one client request!\n");
			//�ѻỰSOCKET��Ϊ������ģʽ
			if ((rtn = ioctlsocket(sessionSocket, FIONBIO, &blockMode) == SOCKET_ERROR)) { //FIONBIO��������ֹ�׽ӿ�s�ķ�����ģʽ��
				cout << "ioctlsocket() failed with error!\n";
				return OK;
			}
			cout << "ioctlsocket() for session socket ok!	Waiting for client connection and data\n";
			//���õȴ��ỰSOKCET�ɽ������ݻ�ɷ�������
			FD_SET(sessionSocket, &rfds);
			FD_SET(sessionSocket, &wfds);
			first_connetion = false;
		}
		//���ỰSOCKET�Ƿ������ݵ���
		if (nTotal > 0) {
			//����ỰSOCKET�����ݵ���������ܿͻ�������
			if (FD_ISSET(sessionSocket, &rfds)) {
				cout << "�Ựsocket�����ݵ�����" << endl;
				//receiving data from client
				std::cout << "request IP:" << (int)clientAddr.sin_addr.S_un.S_un_b.s_b1 << "." << (int)clientAddr.sin_addr.S_un.S_un_b.s_b2 << "." << (int)clientAddr.sin_addr.S_un.S_un_b.s_b3 << "." << (int)clientAddr.sin_addr.S_un.S_un_b.s_b4;
				std::cout << "\nrequest Port:" << clientAddr.sin_port << "\n";
				memset(recvBuf, '\0', 4096);
				rtn = recv(sessionSocket, recvBuf, 4096, 0);
				cout << "�����յ���" << rtn << "���ֽ�" << endl;
				testring(rtn);
				int rt2=sendhead();
				if (rt2 == -1) {
					cout << "ͷ����Ϣ����ʧ�ܣ��رձ������ӡ�"; closesocket(sessionSocket); SOCKET sessionSocket; nTotal = 0;
				}
				if (rtn != -1) {
					cout << "kai shi fa snog xiao xi";
					int rtn1=readrcv();
					if (rtn1 == -1) { cout << "�������������⣬������404����400����" << endl; }
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