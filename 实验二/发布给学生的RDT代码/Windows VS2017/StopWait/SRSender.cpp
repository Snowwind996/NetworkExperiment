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
	if (this->waitingState) { //���ͷ����ڵȴ�ȷ��״̬
		return false;
	}
	if (this->expectSequenceNumberSend < this->base + this->maxnum) {
		this->packetWaitingAck.acknum = -1; //���Ը��ֶ�
		this->packetWaitingAck.seqnum = this->expectSequenceNumberSend;
		this->packetWaitingAck.checksum = 0;
		memcpy(this->packetset[expectSequenceNumberSend - this->base].payload, message.data, sizeof(message.data));
		this->packetset[expectSequenceNumberSend - this->base].seqnum = this->expectSequenceNumberSend;
		this->packetset[expectSequenceNumberSend - this->base].checksum = 0;
		this->packetset[expectSequenceNumberSend - this->base].acknum = -1;
		this->packetset[expectSequenceNumberSend - this->base].checksum = pUtils->calculateCheckSum(this->packetset[expectSequenceNumberSend - this->base]);
		pUtils->printPacket("���ͷ����ͱ���", this->packetset[expectSequenceNumberSend - this->base]);
		pns->sendToNetworkLayer(RECEIVER, this->packetset[expectSequenceNumberSend - this->base]);
		 pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetset[expectSequenceNumberSend - this->base].seqnum);
		//�������ͷ���ʱ��
					
		this->expectSequenceNumberSend++;
		return true;
	}
	this->waitingState = true;

	return false;


}

void SRSender::receive(const Packet& ackPkt) {

	//���У����Ƿ���ȷ
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	printf("���ͷ���У���Ϊ��%d\n", checkSum);
	//���У�����ȷ������ȷ�����=���ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ����
	if (checkSum == ackPkt.checksum && ackPkt.acknum == this->packetset[ackPkt.acknum - base].seqnum) {

		pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
		this->hashtable[ackPkt.acknum] = 1;
		pns->stopTimer(SENDER, ackPkt.acknum);
	}
	while (this->hashtable[this->base] == 1) {
		this->base++; 
		this->packetset++;
	}
	printf("���ͷ��Ļ������:%d\n���ͷ������ţ�%d", this->base, this->expectSequenceNumberSend);
	this->waitingState = false;


}


void SRSender::timeoutHandler(int seqNum) {
	pUtils->printPacket("���ͷ���ʱ��ʱ�䵽���ط��ϴη��͵ı���", this->packetWaitingAck);
	pns->stopTimer(SENDER, seqNum);										//���ȹرն�ʱ��
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//�����������ͷ���ʱ��
	
	printf("���ͷ����ڷ��͵�%d�ű���\n", this->packetset[seqNum-this->base].seqnum);
	pns->sendToNetworkLayer(RECEIVER, this->packetset[seqNum-this->base]);			//���·������ݰ�
	


}
