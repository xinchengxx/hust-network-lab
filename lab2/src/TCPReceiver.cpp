#include "TCPReceiver.h"
#include "Global.h"

TCPReceiver::TCPReceiver(const int N, const int k): N(N), mod(1 << k), base(0) {}


void TCPReceiver::receive(const Packet &packet) {
    int checkSum = pUtils->calculateCheckSum(packet);
    if (checkSum == packet.checksum) {
        pUtils->printPacket("接收方正确收到发送方的报文", packet);
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
                // 冗余ack
                Packet ackPacket;
                ackPacket.acknum = this->base; //确认序号等于收到的报文序号
                ackPacket.checksum = pUtils->calculateCheckSum(ackPacket);
                pUtils->printPacket("接收方发送确认报文", ackPacket);
                pns->sendToNetworkLayer(SENDER, ackPacket);
            }
        }
    }
}