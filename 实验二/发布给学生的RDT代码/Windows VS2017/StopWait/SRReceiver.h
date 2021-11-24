#pragma once

#ifndef SR_RECEIVER_H
#define SR_RECEIVER_H
#include "RdtReceiver.h"
class SRReceiver :public RdtReceiver
{
private:
	int expectSequenceNumberRcvd;	// 期待收到的下一个报文序号
	Packet lastAckPkt;				//上次发送的确认报文
	Packet* packetset;
	int base;
	int maxnum;
	int hashtable[1000];

public:
	SRReceiver(int num,int number);
	virtual ~SRReceiver();

public:

	void receive(const Packet& packet);	//接收报文，将被NetworkService调用
};
#endif


