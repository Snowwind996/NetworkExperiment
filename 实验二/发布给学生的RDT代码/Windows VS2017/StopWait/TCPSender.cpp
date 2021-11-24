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
	if (this->waitingState) { //���ͷ����ڵȴ�ȷ��״̬
		return false;
	}
	if (this->expectSequenceNumberSend < base + this->maxnum) {
		this->packetWaitingAck.acknum = -1; //���Ը��ֶ�
		this->packetWaitingAck.seqnum = this->expectSequenceNumberSend;
		this->packetWaitingAck.checksum = 0;
		memcpy(this->packetset[expectSequenceNumberSend - base].payload, message.data, sizeof(message.data));
		this->packetset[expectSequenceNumberSend - base].seqnum = this->expectSequenceNumberSend;
		this->packetset[expectSequenceNumberSend - base].checksum = 0;
		this->packetset[expectSequenceNumberSend - base].acknum = -1;
		this->packetset[expectSequenceNumberSend - base].checksum = pUtils->calculateCheckSum(this->packetset[expectSequenceNumberSend - base]);
		pUtils->printPacket("���ͷ����ͱ���", this->packetset[expectSequenceNumberSend - base]);
		pns->sendToNetworkLayer(RECEIVER, this->packetset[expectSequenceNumberSend - base]);
		if (base == expectSequenceNumberSend) { pns->startTimer(SENDER, Configuration::TIME_OUT, 1); }
		//�������ͷ���ʱ��
					//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�
		this->expectSequenceNumberSend++;
		return true;
	}
	this->waitingState = true;

	return false;


}

void TCPSender::receive(const Packet& ackPkt) {

	//���У����Ƿ���ȷ
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	printf("���ͷ���У���Ϊ��%d", checkSum);
	//���У�����ȷ������ȷ�����=���ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ����
	if (checkSum == ackPkt.checksum && ackPkt.acknum == this->packetset[ackPkt.acknum - base].seqnum) {
		pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
		this->base = ackPkt.acknum + 1;
		printf("���ͷ��Ļ������:%d\n���ͷ������ţ�%d", this->base, this->expectSequenceNumberSend);
		this->hashtable[this->base]++;
	}
	if (this->hashtable[this->base] == 3) {
		printf("_____________________�Ѿ��ظ��յ���3�����ģ����������ش�����_______________________________\n");
		pns->sendToNetworkLayer(RECEIVER, this->packetset[ackPkt.acknum-this->base]);
		pns->stopTimer(SENDER, 1);
		pns->startTimer(SENDER, Configuration::TIME_OUT, 1);
		
	}
		
	if (this->base - this->packetset[0].seqnum >= 1)			this->packetset++;

	if (this->base == this->expectSequenceNumberSend) {

		pns->stopTimer(SENDER, 1);
	}	//�رն�ʱ��
	else {
		pns->stopTimer(SENDER, 1);
		pns->startTimer(SENDER, Configuration::TIME_OUT, 1);
	}
}


void TCPSender::timeoutHandler(int seqNum) {
	//Ψһһ����ʱ��,���迼��seqNum
	pUtils->printPacket("���ͷ���ʱ��ʱ�䵽���ط��ϴη��͵ı���", this->packetWaitingAck);
	pns->stopTimer(SENDER, seqNum);										//���ȹرն�ʱ��
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//�����������ͷ���ʱ��
	for (int i = 0; i < this->expectSequenceNumberSend - this->base; i++) {
		printf("���ͷ����ڷ��͵�%d�ű���\n", this->packetset[i].seqnum);
		pns->sendToNetworkLayer(RECEIVER, this->packetset[i]);			//���·������ݰ�
	}


}
