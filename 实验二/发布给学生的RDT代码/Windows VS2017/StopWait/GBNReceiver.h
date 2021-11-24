#pragma once

#ifndef GBN_RECEIVER_H
#define GBN_RECEIVER_H
#include "RdtReceiver.h"
class GBNReceiver :public RdtReceiver
{
private:
	int expectSequenceNumberRcvd;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���

public:
	GBNReceiver(int num);
	virtual ~GBNReceiver();

public:

	void receive(const Packet& packet);	//���ձ��ģ�����NetworkService����
};
#endif


