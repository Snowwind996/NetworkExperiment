#include "stdafx.h"
#include "Global.h"
#include "TCPSender.h"


TCPSender::TCPSender(int num, int maxnum)
{
	this->expectSequenceNumberSend = num;
	this->waitingState = false;
	this->base = this->expectSequenceNumberSend;
	this->maxnum = maxnum;
	this->packetset = (Packet*)malloc(1000 * sizeof(Packet));
	for (int i = 0; i < 1000; i++) {
		this->hashtable[i] = 0;
	}
	
}


TCPSender::~TCPSender()
{
}



bool TCPSender::getWaitingState() {
	return waitingState;
}




bool TCPSender::send(const Message& message) {
	if (this->waitingState) { //发送方处于等待确认状态
		return false;
	}
	if (this->expectSequenceNumberSend < base + this->maxnum) {
		this->packetWaitingAck.acknum = -1; //忽略该字段
		this->packetWaitingAck.seqnum = this->expectSequenceNumberSend;
		this->packetWaitingAck.checksum = 0;
		memcpy(this->packetset[expectSequenceNumberSend - base].payload, message.data, sizeof(message.data));
		this->packetset[expectSequenceNumberSend - base].seqnum = this->expectSequenceNumberSend;
		this->packetset[expectSequenceNumberSend - base].checksum = 0;
		this->packetset[expectSequenceNumberSend - base].acknum = -1;
		this->packetset[expectSequenceNumberSend - base].checksum = pUtils->calculateCheckSum(this->packetset[expectSequenceNumberSend - base]);
		pUtils->printPacket("发送方发送报文", this->packetset[expectSequenceNumberSend - base]);
		pns->sendToNetworkLayer(RECEIVER, this->packetset[expectSequenceNumberSend - base]);
		if (base == expectSequenceNumberSend) { pns->startTimer(SENDER, Configuration::TIME_OUT, 1); }
		//启动发送方定时器
					//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
		this->expectSequenceNumberSend++;
		return true;
	}
	this->waitingState = true;

	return false;


}

void TCPSender::receive(const Packet& ackPkt) {

	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	printf("发送方的校验和为：%d", checkSum);
	//如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
	if (checkSum == ackPkt.checksum && ackPkt.acknum == this->packetset[ackPkt.acknum - base].seqnum) {
		pUtils->printPacket("发送方正确收到确认", ackPkt);
		this->base = ackPkt.acknum + 1;
		printf("发送方的基序序号:%d\n发送方最大序号：%d", this->base, this->expectSequenceNumberSend);
		this->hashtable[this->base]++;
	}
	if (this->hashtable[this->base] == 3) {
		printf("_____________________已经重复收到了3个报文，启动快速重传机制_______________________________\n");
		pns->sendToNetworkLayer(RECEIVER, this->packetset[ackPkt.acknum-this->base]);
		pns->stopTimer(SENDER, 1);
		pns->startTimer(SENDER, Configuration::TIME_OUT, 1);
		
	}
		
	if (this->base - this->packetset[0].seqnum >= 1)			this->packetset++;

	if (this->base == this->expectSequenceNumberSend) {

		pns->stopTimer(SENDER, 1);
	}	//关闭定时器
	else {
		pns->stopTimer(SENDER, 1);
		pns->startTimer(SENDER, Configuration::TIME_OUT, 1);
	}
}


void TCPSender::timeoutHandler(int seqNum) {
	//唯一一个定时器,无需考虑seqNum
	pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", this->packetWaitingAck);
	pns->stopTimer(SENDER, seqNum);										//首先关闭定时器
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器
	for (int i = 0; i < this->expectSequenceNumberSend - this->base; i++) {
		printf("发送方正在发送第%d号报文\n", this->packetset[i].seqnum);
		pns->sendToNetworkLayer(RECEIVER, this->packetset[i]);			//重新发送数据包
	}


}
