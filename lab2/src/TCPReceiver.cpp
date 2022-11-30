#include "TCPReceiver.h"
#include "Global.h"

TCPReceiver::TCPReceiver(const int N, const int k): N(N), mod(1 << k), base(0), buffer(1 << k), cachedPkts(1 << k) {}


void TCPReceiver::receive(const Packet &packet) {
    int checkSum = pUtils->calculateCheckSum(packet);
    if (checkSum == packet.checksum) {
        pUtils->printPacket("接收方正确收到发送方的报文", packet);
        if ((packet.seqnum - base + mod) % mod < N) {
            if (packet.seqnum == base) {
                Message msg;
                this->buffer[base] = true;
                this->cachedPkts[base] = packet;
                memcpy(this->cachedPkts[base].payload, packet.payload, sizeof(packet.payload));
                int i = 0;
                while (i < N) {
                    if (this->buffer[(base + i) % mod] == true) {
                        memcpy(msg.data, this->cachedPkts[(base + i) % mod].payload, sizeof(this->cachedPkts[(base + i) % mod].payload));
                        pns->delivertoAppLayer(RECEIVER, msg);
                        this->buffer[(base + i) % mod] = false;
                        i++;
                    } else {
                        break;
                    }
                }
                this->base = (this->base + i) % mod;
                Packet ackPacket;
                ackPacket.acknum = (this->base - 1 + mod) % mod;
                ackPacket.seqnum = -1;
                for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
                    ackPacket.payload[i] = '.';
                }
                ackPacket.checksum = pUtils->calculateCheckSum(ackPacket);
                pUtils->printPacket("接收方发送确认报文", ackPacket);
                pns->sendToNetworkLayer(SENDER, ackPacket);
            } else {
                this->cachedPkts[packet.seqnum] = packet;
                this->buffer[packet.seqnum] = true;
                memcpy(this->cachedPkts[base].payload, packet.payload, sizeof(packet.payload));
                // 冗余ack
                Packet ackPacket;
                ackPacket.acknum = (this->base - 1 + mod) % mod; //确认序号等于收到的报文序号
                ackPacket.seqnum = -1;
                for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
                    ackPacket.payload[i] = '.';
                }
                ackPacket.checksum = pUtils->calculateCheckSum(ackPacket);
                pUtils->printPacket("接收方发送冗余ack", ackPacket);
                pns->sendToNetworkLayer(SENDER, ackPacket);
            }
        } else if ((packet.seqnum - (base - N + mod) % mod + mod) % mod < N) {
            Packet ackPacket;
            ackPacket.acknum = packet.seqnum;
            //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
            ackPacket.seqnum = -1;
            for (int i = 0; i < Configuration::PAYLOAD_SIZE;i++) {
                ackPacket.payload[i] = '.';
            }
            ackPacket.checksum = pUtils->calculateCheckSum(ackPacket);
            pUtils->printPacket("接收方发送确认报文", ackPacket);
            pns->sendToNetworkLayer(SENDER, ackPacket);
        }
    }
}