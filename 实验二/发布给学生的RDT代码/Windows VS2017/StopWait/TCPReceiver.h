#pragma once

#ifndef TCP_RECEIVER_H
#define TCP_RECEIVER_H
#include "RdtReceiver.h"
class TCPReceiver :public RdtReceiver
{
private:
	int expectSequenceNumberRcvd;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���

public:
	TCPReceiver(int num);
	virtual ~TCPReceiver();

public:

	void receive(const Packet& packet);	//���ձ��ģ�����NetworkService����
};
#endif


