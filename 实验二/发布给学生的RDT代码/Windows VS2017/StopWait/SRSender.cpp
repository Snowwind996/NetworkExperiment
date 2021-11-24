#include "stdafx.h"
#include "Global.h"
#include "SRSender.h"


SRSender::SRSender(int num, int number)
{
	this->expectSequenceNumberSend = num;
	this->waitingState = false;
	this->base = this->expectSequenceNumberSend;
	this->maxnum = number;
	this->packetset = (Packet*)malloc(1000 * sizeof(Packet));
	for (int i = 0; i < 1000; i++) {
		this->hashtable[i] = 0;
	}
	

}


SRSender::~SRSender()
{
}



bool SRSender::getWaitingState() {
	return waitingState;
}




bool SRSender::send(const Message& message) {
	if (this->waitingState) { //发送方处于等待确认状态
		return false;
	}
	if (this->expectSequenceNumberSend < this->base + this->maxnum) {
		this->packetWaitingAck.acknum = -1; //忽略该字段
		this->packetWaitingAck.seqnum = this->expectSequenceNumberSend;
		this->packetWaitingAck.checksum = 0;
		memcpy(this->packetset[expectSequenceNumberSend - this->base].payload, message.data, sizeof(message.data));
		this->packetset[expectSequenceNumberSend - this->base].seqnum = this->expectSequenceNumberSend;
		this->packetset[expectSequenceNumberSend - this->base].checksum = 0;
		this->packetset[expectSequenceNumberSend - this->base].acknum = -1;
		this->packetset[expectSequenceNumberSend - this->base].checksum = pUtils->calculateCheckSum(this->packetset[expectSequenceNumberSend - this->base]);
		pUtils->printPacket("发送方发送报文", this->packetset[expectSequenceNumberSend - this->base]);
		pns->sendToNetworkLayer(RECEIVER, this->packetset[expectSequenceNumberSend - this->base]);
		 pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetset[expectSequenceNumberSend - this->base].seqnum);
		//启动发送方定时器
					
		this->expectSequenceNumberSend++;
		return true;
	}
	this->waitingState = true;

	return false;


}

void SRSender::receive(const Packet& ackPkt) {

	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	printf("发送方的校验和为：%d\n", checkSum);
	//如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
	if (checkSum == ackPkt.checksum && ackPkt.acknum == this->packetset[ackPkt.acknum - base].seqnum) {

		pUtils->printPacket("发送方正确收到确认", ackPkt);
		this->hashtable[ackPkt.acknum] = 1;
		pns->stopTimer(SENDER, ackPkt.acknum);
	}
	while (this->hashtable[this->base] == 1) {
		this->base++; 
		this->packetset++;
	}
	printf("发送方的基序序号:%d\n发送方最大序号：%d", this->base, this->expectSequenceNumberSend);
	this->waitingState = false;


}


void SRSender::timeoutHandler(int seqNum) {
	pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", this->packetWaitingAck);
	pns->stopTimer(SENDER, seqNum);										//首先关闭定时器
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器
	
	printf("发送方正在发送第%d号报文\n", this->packetset[seqNum-this->base].seqnum);
	pns->sendToNetworkLayer(RECEIVER, this->packetset[seqNum-this->base]);			//重新发送数据包
	


}
