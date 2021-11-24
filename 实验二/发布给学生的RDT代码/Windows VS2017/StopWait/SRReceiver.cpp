#include "stdafx.h"
#include "Global.h"
#include "SRReceiver.h"


SRReceiver::SRReceiver(int num,int number)
{
	this->base = num;

	lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//忽略该字段
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
	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(packet);

	//如果校验和正确，同时收到报文的序号等于接收方期待收到的报文序号一致
	if (checkSum == packet.checksum  ) {
		pUtils->printPacket("接收方正确收到发送方的报文", packet);
		
		
		
		//记入接收方缓存
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
			printf("报文在分组之外，忽略该报文");
			return;
		}
		else {
		printf("报文在滑动窗口里面，所以返回确认报文\n");
		}

		lastAckPkt.acknum = packet.seqnum; //确认序号等于收到的报文序号
		lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
		pUtils->printPacket("接收方发送确认报文", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方

		printf("接收方期待收到的报文序号:%d\n", this->base);
	}
	else {
		if (checkSum != packet.checksum) {
			pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
		}
		pUtils->printPacket("接收方重新发送上次的确认报文", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送上次的确认报文

	}
}