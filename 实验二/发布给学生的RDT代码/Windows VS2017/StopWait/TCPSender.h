#include "RdtSender.h"
#pragma once
#ifndef TCP_SENDER_H
#define TCP_SENDER_H
class TCPSender :public RdtSender
{
private:
	int expectSequenceNumberSend;	// ��һ��������� 
	bool waitingState;				// �Ƿ��ڵȴ�Ack��״̬
	Packet packetWaitingAck;		//�ѷ��Ͳ��ȴ�Ack�����ݰ�
	int maxnum;
	Packet* packetset;
	int base;
	int hashtable[1000];
public:

	bool getWaitingState();
	bool send(const Message& message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(const Packet& ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����
	TCPSender(int num, int maxnum);
	virtual ~TCPSender();
};
#endif