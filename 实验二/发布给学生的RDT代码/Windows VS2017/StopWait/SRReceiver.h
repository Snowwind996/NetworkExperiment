#pragma once

#ifndef SR_RECEIVER_H
#define SR_RECEIVER_H
#include "RdtReceiver.h"
class SRReceiver :public RdtReceiver
{
private:
	int expectSequenceNumberRcvd;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���
	Packet* packetset;
	int base;
	int maxnum;
	int hashtable[1000];

public:
	SRReceiver(int num,int number);
	virtual ~SRReceiver();

public:

	void receive(const Packet& packet);	//���ձ��ģ�����NetworkService����
};
#endif


