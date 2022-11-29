#include "SRRdtReceiver.h"
#include "Global.h"

SRRdtReceiver::SRRdtReceiver(const int N, const int k): N(N), mod(1 << k), AckPkts(1 << k), base(0), buffer(1 << k){

}

void SRRdtReceiver::receive(const Packet &packet) {
    int checkSum = pUtils->calculateCheckSum(packet);
    if (checkSum == packet.checksum) {
        pUtils->printPacket("接收方正确收到发送方的报文", packet);
        int last_base = base;
        if ((packet.seqnum - base + mod) % mod < N) {
            if (packet.seqnum == base) {
                Message msg;
                this->buffer[base] = true;
                this->AckPkts[base] = packet;
                memcpy(this->AckPkts[base].payload, packet.payload, sizeof(packet.payload));
                int i;
                while (i < N) {
                    if (this->buffer[(base + i) % mod] == true) {
                        memcpy(msg.data, this->AckPkts[(base + i) % mod].payload, sizeof(this->AckPkts[(base + i) % mod].payload));
                        pns->delivertoAppLayer(RECEIVER, msg);
                        this->buffer[(base + i) % mod] = false;
                        i++;
                    } else {
                        break;
                    }
                }
                this->base = (this->base + i) % mod;
            } else {
                this->AckPkts[packet.seqnum] = packet;
                this->buffer[packet.seqnum] = true;
                memcpy(this->AckPkts[base].payload, packet.payload, sizeof(packet.payload));
            }
            Packet ackPacket;
            ackPacket.acknum = packet.seqnum;
            //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
            ackPacket.seqnum = -1;
            for(int i = 0; i < Configuration::PAYLOAD_SIZE;i++){
              ackPacket.payload[i] = '.';
            }
            ackPacket.checksum = pUtils->calculateCheckSum(ackPacket);
            pUtils->printPacket("接收方发送确认报文", ackPacket);
            pns->sendToNetworkLayer(SENDER, ackPacket);
        } else if ((packet.seqnum - (base - N + mod) % mod + mod) % mod < N) {
            Packet ackPacket;
            ackPacket.acknum = packet.seqnum;
            ackPacket.acknum = packet.seqnum;
            //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
            ackPacket.seqnum = -1;
            for(int i = 0; i < Configuration::PAYLOAD_SIZE;i++){
                ackPacket.payload[i] = '.';
            }
            ackPacket.checksum = pUtils->calculateCheckSum(ackPacket);
            pUtils->printPacket("接收方发送确认报文", ackPacket);
            pns->sendToNetworkLayer(SENDER, ackPacket);
        }
    } else {
        if (checkSum != packet.checksum) {
            pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
        }
        else {
            pUtils->printPacket("接收方没有正确收到发送方的报文,报文序号不对", packet);
        }
    }
}