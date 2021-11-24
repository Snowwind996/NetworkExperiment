#pragma once

#ifndef TCP_RECEIVER_H
#define TCP_RECEIVER_H
#include "RdtReceiver.h"
class TCPReceiver :public RdtReceiver
{
private:
	int expectSequenceNumberRcvd;	// 期待收到的下一个报文序号
	Packet lastAckPkt;				//上次发送的确认报文

public:
	TCPReceiver(int num);
	virtual ~TCPReceiver();

public:

	void receive(const Packet& packet);	//接收报文，将被NetworkService调用
};
#endif


