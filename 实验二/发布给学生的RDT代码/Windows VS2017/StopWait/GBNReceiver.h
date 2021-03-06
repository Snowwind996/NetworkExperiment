#pragma once

#ifndef GBN_RECEIVER_H
#define GBN_RECEIVER_H
#include "RdtReceiver.h"
class GBNReceiver :public RdtReceiver
{
private:
	int expectSequenceNumberRcvd;	// 期待收到的下一个报文序号
	Packet lastAckPkt;				//上次发送的确认报文

public:
	GBNReceiver(int num);
	virtual ~GBNReceiver();

public:

	void receive(const Packet& packet);	//接收报文，将被NetworkService调用
};
#endif


