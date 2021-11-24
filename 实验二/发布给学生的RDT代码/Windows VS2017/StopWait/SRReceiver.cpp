#include "stdafx.h"
#include "Global.h"
#include "SRReceiver.h"


SRReceiver::SRReceiver(int num,int number)
{
	this->base = num;

	lastAckPkt.acknum = -1; //��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����ܵ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//���Ը��ֶ�
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
	for (int i = 0; i < 1000; i++) {
		this->hashtable[i] = 0;
	}
	this->maxnum = number;
	this->packetset = (Packet*)malloc(sizeof(Packet) * 1000);
}


SRReceiver::~SRReceiver()
{
}

void SRReceiver::receive(const Packet& packet) {
	//���У����Ƿ���ȷ
	int checkSum = pUtils->calculateCheckSum(packet);

	//���У�����ȷ��ͬʱ�յ����ĵ���ŵ��ڽ��շ��ڴ��յ��ı������һ��
	if (checkSum == packet.checksum  ) {
		pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);
		
		
		
		//������շ�����
		if (packet.seqnum >= this->base && packet.seqnum < this->base + this->maxnum) {
			memcpy(this->packetset[packet.seqnum - this->base].payload, packet.payload, sizeof(packet.payload));
			this->packetset[packet.seqnum - this->base].seqnum = packet.seqnum;
			this->packetset[packet.seqnum - this->base].checksum = 0;
			this->packetset[packet.seqnum - this->base].acknum = -1;
			this->packetset[packet.seqnum - this->base].checksum = pUtils->calculateCheckSum(this->packetset[packet.seqnum - this->base]);
			this->hashtable[packet.seqnum] = 1;
			while (this->hashtable[this->base] == 1) {
				Message msg;
				memcpy(msg.data, packetset[0].payload, sizeof(this->packetset[0].payload));
				pns->delivertoAppLayer(RECEIVER, msg);
				this->base++;
				this->packetset++;
			}
			

		}
		else if (packet.seqnum>=this->base+this->maxnum){
			printf("�����ڷ���֮�⣬���Ըñ���");
			return;
		}
		else {
		printf("�����ڻ����������棬���Է���ȷ�ϱ���\n");
		}

		lastAckPkt.acknum = packet.seqnum; //ȷ����ŵ����յ��ı������
		lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
		pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�

		printf("���շ��ڴ��յ��ı������:%d\n", this->base);
	}
	else {
		if (checkSum != packet.checksum) {
			pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
		}
		pUtils->printPacket("���շ����·����ϴε�ȷ�ϱ���", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢���ϴε�ȷ�ϱ���

	}
}